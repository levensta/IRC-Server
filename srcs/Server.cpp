#include "Server.hpp"

Server::Server(int port, const std::string &password) :
port(port), timeout(1), password(password), name("IRCat")
{
	commands["PASS"] = &Server::passCmd;
	commands["NICK"] = &Server::nickCmd;
	commands["USER"] = &Server::userCmd;
	commands["QUIT"] = &Server::quitCmd;
	commands["PRIVMSG"] = &Server::privmsgCmd;
	commands["AWAY"] = &Server::awayCmd;
	commands["NOTICE"] = &Server::noticeCmd;
	commands["WHO"] = &Server::whoCmd;
	commands["WHOIS"] = &Server::whoisCmd;
	commands["WHOWAS"] = &Server::whowasCmd;
	commands["MODE"] = &Server::modeCmd;
	commands["TOPIC"] = &Server::topicCmd;
	commands["JOIN"] = &Server::joinCmd;
	commands["INVITE"] = &Server::inviteCmd;
	commands["KICK"] = &Server::kickCmd;
	commands["PART"] = &Server::partCmd;
	commands["NAMES"] = &Server::namesCmd;
	commands["LIST"] = &Server::listCmd;
	commands["WALLOPS"] = &Server::wallopsCmd;
	commands["ISON"] = &Server::isonCmd;
	commands["USERHOST"] = &Server::userhostCmd;
	commands["TIME"] = &Server::timeCmd;
	commands["PING"] = &Server::pingCmd;
	commands["PONG"] = &Server::pongCmd;

	// Read MOTD
	std::string		line;
	std::ifstream	motdFile("conf/IRCat.motd");
	if (motdFile.is_open())
	{
		while (getline(motdFile, line))
			motd.push_back(line);
		motdFile.close();
	}
}

Server::~Server()
{
	for (size_t i = 0; i < connectedUsers.size(); ++i)
	{
		close(connectedUsers[i]->getSockfd());
		delete connectedUsers[i];
	}
	std::map<std::string, Channel *>::const_iterator	beg = channels.begin();
	std::map<std::string, Channel *>::const_iterator	end = channels.end();
	for (; beg != end; ++beg)
		delete (*beg).second;
	close(sockfd);
}

const int	&Server::getSockfd() const
{
	return (sockfd);
}

User	*Server::getUserByName(const std::string &name)
{
	User	*ret;
	size_t	usersCount = connectedUsers.size();
	for (size_t i = 0; i < usersCount; i++)
		if (connectedUsers[i]->getNickname() == name)
			ret = connectedUsers[i];
	return ret;
}

bool	Server::containsNickname(const std::string &nickname) const
{
	size_t	usersCount = connectedUsers.size();
	for (size_t i = 0; i < usersCount; i++)
	{
		if (connectedUsers[i]->getNickname() == nickname)
			return (true);
	}
	return (false);
}

bool	Server::containsChannel(const std::string &name) const
{
	try
	{
		channels.at(name);
		return true;
	}
	catch(const std::exception& e)
	{}
	return false;
}

void	Server::createSocket()
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
}

void	Server::bindSocket()
{
	const int trueFlag = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &trueFlag, sizeof(int)) < 0)
	{
		std::cout << "setsockopt failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(port); // htons is necessary to convert a number to network byte order
	if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
	{
		std::cout << "Failed to bind to port " << port << ". errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
}

void	Server::listenSocket()
{
	if (listen(sockfd, 128) < 0)
	{
		std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
}

void	Server::grabConnection()
{
	size_t addrlen = sizeof(sockaddr);
	int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
	if (connection >= 0)
	{
		struct pollfd	pfd;
		pfd.fd = connection;
		pfd.events = POLLIN;
		pfd.revents = 0;
		userFDs.push_back(pfd);
		connectedUsers.push_back(new User(connection));
	}
}

void	Server::processMessages()
{
	int	pret = poll(userFDs.data(), userFDs.size(), timeout);
	std::vector<int>	toErase;
	if (pret != 0)
	{
		// Read from the connection
		for (size_t i = 0; i < userFDs.size(); i++)
		{
			int ret = 0;
			if (userFDs[i].revents & POLLIN)
			{
				connectedUsers[i]->readMessage();
				if ((ret = hadleMessages(*(connectedUsers[i]))) == DISCONNECT)
					toErase.push_back(i - toErase.size());
			}
			if (this->connectedUsers[i]->getFlags() & REGISTERED)
			{
				if (time(0) - this->connectedUsers[i]->getTimeOfLastMessage() > 120 ) // время взять из конфига todo
				{
					this->connectedUsers[i]->sendMessage(":" + this->name + " PING :" + this->name + "\n");
					this->connectedUsers[i]->updateTimeAfterPing();
					this->connectedUsers[i]->updateTimeOfLastMessage();
					this->connectedUsers[i]->setFlag(PINGING);
				}
				if (ret != DISCONNECT && this->breakUnresponsiveConnection(*(connectedUsers[i])) == DISCONNECT)
					toErase.push_back(i - toErase.size());
			}
			userFDs[i].revents = 0;
		}
		// Delete broken connections
		for (size_t i = 0; i < toErase.size(); i++)
		{
			size_t	pos = toErase[i];
			close(connectedUsers[pos]->getSockfd());
			std::map<std::string, Channel *>::iterator	beg = channels.begin();
			std::map<std::string, Channel *>::iterator	end = channels.end();
			for (; beg != end; ++beg)
			{
				if ((*beg).second->containsNickname(connectedUsers[pos]->getNickname()))
				{
					(*beg).second->sendMessage("QUIT :" + connectedUsers[pos]->getQuitMessage() + "\n", *(connectedUsers[pos]), true);
					(*beg).second->disconnect(*(connectedUsers[pos]));
				}
			}
			delete connectedUsers[pos];
			connectedUsers.erase(connectedUsers.begin() + pos);
			userFDs.erase(userFDs.begin() + pos);
		}
		// Delete empty channels
		std::map<std::string, Channel *>::const_iterator	beg = channels.begin();
		std::map<std::string, Channel *>::const_iterator	end = channels.end();
		for (; beg != end;)
		{
			if ((*beg).second->isEmpty())
			{
				channels.erase(beg);
				beg = channels.begin();
			}
			else
				++beg;
		}
	}
}

void	Server::sendMOTD(const User &user) const
{
	if (motd.size() == 0)
		sendError(user, ERR_NOMOTD);
	else
	{
		sendReply(name, user, RPL_MOTDSTART, name);
		for (size_t i = 0; i < motd.size(); ++i)
			sendReply(name, user, RPL_MOTD, motd[i]);
		sendReply(name, user, RPL_ENDOFMOTD);
	}
}

int		Server::connectToChannel(const User &user, const std::string &name, const std::string &key)
{
	try
	{
		Channel	*tmp = channels.at(name);
		tmp->connect(user, key);
		return (1);
	}
	catch(const std::exception& e)
	{
		channels[name] = new Channel(name, user, key);
	}
	return (1);
}

int		Server::hadleMessages(User &user)
{
	while (user.getMessages().size() > 0 \
			&& user.getMessages().front()[user.getMessages().front().size() - 1] == '\n')
	{
		Message	msg(user.getMessages().front());
		user.popMessage();
		// log message to server console
		logMessage(msg);
		// handle
		if (!(user.getFlags() & REGISTERED) && msg.getCommand() != "QUIT" && msg.getCommand() != "PASS" \
				&& msg.getCommand() != "USER" && msg.getCommand() != "NICK")
			sendError(user, ERR_NOTREGISTERED);
		else
		{
			try
			{
				int ret = (this->*(commands.at(msg.getCommand())))(msg, user);
				if (ret == DISCONNECT)
					return (DISCONNECT);
			}
			catch(const std::exception& e)
			{
				sendError(user, ERR_UNKNOWNCOMMAND, msg.getCommand());
			}
			
		}
	}
	user.updateTimeOfLastMessage();
	return (0);
}

int		Server::breakUnresponsiveConnection(const User &user)
{
	std::cout << 1 << std::endl;
	if ((user.getFlags() & PINGING) && time(0) - user.getTimeAfterPing() > 60) // время взять из конфига todo
		return DISCONNECT;
	std::cout << 2 << std::endl;
	return 0;
}