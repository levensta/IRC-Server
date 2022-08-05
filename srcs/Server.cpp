#include "Server.hpp"

Server::Server(int port, const std::string &password) :
port(port), timeout(1), password(password)
{
	commands["PASS"] = &Server::passCmd;
	commands["NICK"] = &Server::nickCmd;
	commands["USER"] = &Server::userCmd;
	commands["OPER"] = &Server::operCmd;
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
	commands["PING"] = &Server::pingCmd;
	commands["PONG"] = &Server::pongCmd;
	commands["ISON"] = &Server::isonCmd;
	commands["USERHOST"] = &Server::userhostCmd;
	commands["VERSION"] = &Server::versionCmd;
	commands["INFO"] = &Server::infoCmd;
	commands["ADMIN"] = &Server::adminCmd;
	commands["TIME"] = &Server::timeCmd;
	commands["REHASH"] = &Server::rehashCmd;
	commands["RESTART"] = &Server::restartCmd;
	commands["KILL"] = &Server::killCmd;

	// Read MOTD
	std::string		line;
	std::ifstream	motdFile("conf/IRCat.motd");
	if (motdFile.is_open())
	{
		while (getline(motdFile, line))
			motd.push_back(line);
		motdFile.close();
	}
	
	loadConfig();
	//Check config for set correct values ?
}

void Server::loadConfig() {
	
	static bool wasLoaded = false;

	JSON::Object *conf = NULL;
	try {
		conf = JSON::parseFile("conf/IRConf.json");
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}

	if (conf != NULL) {

		//Getting values from config
		name = conf->get("servername")->toStr();
		info = conf->get("info")->toStr();
		version = conf->get("version")->toStr();
		debuglvl = conf->get("debuglvl")->toStr();
		comments = conf->get("comments")->toStr();
		describe = conf->get("describe")->toStr();
		adminName = conf->get("adminName")->toStr();
		adminEmail = conf->get("adminEmail")->toStr();
		adminNickname = conf->get("adminNickname")->toStr();
		allowedIP = inet_addr(conf->get("allowedIP")->toStr().c_str());
		maxChannels = static_cast<unsigned long>(conf->get("maxChannels")->toNum());
		maxInactiveTimeout = static_cast<unsigned long>(conf->get("maxInactiveTimeout")->toNum());
		maxResponseTimeout = static_cast<unsigned long>(conf->get("maxResponseTimeout")->toNum());
		
		operators.clear();
		fillOperatorsList(operators, conf->get("operators")->toObj());

		delete conf;
		wasLoaded = true;
	}
	else if (wasLoaded != true) {

		//Set defaults
		name = "IRCat";
		info = "None";
		version = "None";
		debuglvl = "None";
		comments = "None";
		describe = "None";
		adminName = "None";
		adminEmail = "None";
		adminNickname = "None";
		allowedIP = 0UL;
		maxChannels = 10;
		maxInactiveTimeout = 120;
		maxResponseTimeout = 60;
	}

	//Only for debug
	std::cout << "CONFIG" << std::endl;
	std::cout << "servername: " << name << std::endl;
	std::cout << "info: " << info << std::endl;
	std::cout << "version: " << version << std::endl;
	std::cout << "debuglvl: " << debuglvl << std::endl;
	std::cout << "comments: " << comments << std::endl;
	std::cout << "describe: " << describe << std::endl;
	std::cout << "adminName: " << adminName << std::endl;
	std::cout << "adminEmail: " << adminEmail << std::endl;
	std::cout << "adminNickname: " << adminNickname << std::endl;
	std::cout << "maxChannels: " << maxChannels << std::endl;	
	std::cout << "maxInactiveTimeout: " << maxInactiveTimeout << std::endl;	
	std::cout << "maxResponseTimeout: " << maxResponseTimeout << std::endl;	
	
	struct in_addr paddr;
	paddr.s_addr = allowedIP;
	std::cout << "allowedIP(int): " << allowedIP << std::endl;
	std::cout << "allowedIP(str): " << inet_ntoa(paddr) << std::endl;

	std::map<std::string, std::string>::iterator beg = operators.begin();
	std::map<std::string, std::string>::iterator end = operators.end();
	std::map<std::string, std::string>::iterator it;

	for (it = beg; it != end; it++)	{
		std::cout << "Login: " << it->first << " " << "Hash: " << it->second << std::endl;
	}
}

void Server::fillOperatorsList(std::map<std::string, std::string> &operators, JSON::Object *confOperators) {
	std::map<std::string, JSON::AType *>::iterator beg = confOperators->begin();
	std::map<std::string, JSON::AType *>::iterator end = confOperators->end();
	std::map<std::string, JSON::AType *>::iterator it;

	for (it = beg; it != end; it++)
	{
		if (it->second != NULL)
		{
			operators.insert(std::pair<std::string, std::string>(it->first, it->second->toStr()));
		}
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
	sockaddr.sin_addr.s_addr = allowedIP; // INADDR_ANY; Was 0.0.0.0, now 127.0.0.1
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
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
}

void	Server::grabConnection()
{
	//std::cout<< connectedUsers.size() << std::endl;

	size_t addrlen = sizeof(sockaddr);
	int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
	if (connection >= 0)
	{
		char	host[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(sockaddr.sin_addr), host, INET_ADDRSTRLEN);
		struct pollfd	pfd;
		pfd.fd = connection;
		pfd.events = POLLIN;
		pfd.revents = 0;
		userFDs.push_back(pfd);
		connectedUsers.push_back(new User(connection, host, name));
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
			if (userFDs[i].revents & POLLIN)
			{
				if (connectedUsers[i]->readMessage() == DISCONNECT)
					connectedUsers[i]->setFlag(BREAKCONNECTION);
				else if (hadleMessages(*(connectedUsers[i])) == DISCONNECT)
					connectedUsers[i]->setFlag(BREAKCONNECTION);
			}
			userFDs[i].revents = 0;
		}
	}
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

void	Server::notifyUsers(User &user, const std::string &notification)
{
	const std::vector<const Channel *> chans = user.getChannels();
	for (size_t i = 0; i < connectedUsers.size(); i++)
	{
		for (size_t j = 0; j < chans.size(); j++)
		{
			if (chans[j]->containsNickname(connectedUsers[i]->getNickname()))
			{
				connectedUsers[i]->sendMessage(notification);
				break ;
			}
		}
	}
}

void	Server::deleteBrokenConnections()
{
	for (size_t i = 0; i < connectedUsers.size(); ++i)
	{
		if (connectedUsers[i]->getFlags() & BREAKCONNECTION)
		{
			this->nicknamesHistory.addUser(*(connectedUsers[i]));
			notifyUsers(*(connectedUsers[i]), ":" + connectedUsers[i]->getPrefix() + " QUIT :" + connectedUsers[i]->getQuitMessage() + "\n");
			close(connectedUsers[i]->getSockfd());
			std::map<std::string, Channel *>::iterator	beg = channels.begin();
			std::map<std::string, Channel *>::iterator	end = channels.end();
			for (; beg != end; ++beg)
				if ((*beg).second->containsNickname(connectedUsers[i]->getNickname()))
					(*beg).second->disconnect(*(connectedUsers[i]));
			delete connectedUsers[i];
			connectedUsers.erase(connectedUsers.begin() + i);
			userFDs.erase(userFDs.begin() + i);
			--i;
		}
	}
}

void	Server::deleteEmptyChannels()
{
	std::map<std::string, Channel *>::const_iterator	beg = channels.begin();
	std::map<std::string, Channel *>::const_iterator	end = channels.end();
	for (; beg != end;)
	{
		if ((*beg).second->isEmpty())
		{
			delete (*beg).second;
			channels.erase((*beg).first);
			beg = channels.begin();
		}
		else
			++beg;
	}
}

void	Server::checkConnectionWithUsers()
{
	for (size_t i = 0; i < connectedUsers.size(); i++)
	{
		if (this->connectedUsers[i]->getFlags() & REGISTERED)
		{
			if (time(0) - this->connectedUsers[i]->getTimeOfLastMessage() > static_cast<time_t>(maxInactiveTimeout) )
			{
				this->connectedUsers[i]->sendMessage(":" + this->name + " PING :" + this->name + "\n");
				this->connectedUsers[i]->updateTimeAfterPing();
				this->connectedUsers[i]->updateTimeOfLastMessage();
				this->connectedUsers[i]->setFlag(PINGING);
			}
			if ((connectedUsers[i]->getFlags() & PINGING) && time(0) - connectedUsers[i]->getTimeAfterPing() > static_cast<time_t>(maxResponseTimeout) )
				connectedUsers[i]->setFlag(BREAKCONNECTION);
		}
	}
}
