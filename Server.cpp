#include "Server.hpp"

Server::Server(int port, const std::string &password) :
port(port), timeout(20), password(password), name("IRCat")
{
	// Read MOTD
	std::string		line;
	std::ifstream	motdFile("IRCat.motd");
	if (motdFile.is_open())
	{
		while (getline(motdFile, line))
			motd.push_back(line);
		motdFile.close();
	}
}

Server::~Server()
{}

const int	&Server::getSockfd() const
{
	return (sockfd);
}

const std::string						&Server::getPassword() const
{
	return (password);
}

const std::string						&Server::getServername() const
{
	return (name);
}

const std::map<std::string, Channel *>	&Server::getChannels() const
{
	return (channels);
}

bool									Server::containsNickname(const std::string &nickname) const
{
	size_t	usersCount = connectedUsers.size();
	for (size_t i = 0; i < usersCount; i++)
	{
		if (connectedUsers[i]->getNickname() == nickname)
			return (true);
	}
	return (false);
}

void									Server::createSocket()
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
}

void									Server::bindSocket()
{
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(port); // htons is necessary to convert a number to network byte order
	if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
	{
		std::cout << "Failed to bind to port " << port << ". errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
}

void									Server::listenSocket()
{
	if (listen(sockfd, 128) < 0)
	{
		std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
}

void									Server::grabConnection()
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
		connectedUsers.push_back(new User(connection, *this));
	}
}

void									Server::processMessages()
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
				connectedUsers[i]->readMessage();
				if (connectedUsers[i]->hadleMessages() == -1)
					toErase.push_back(i - toErase.size());
			}
			userFDs[i].revents = 0;
		}
		// Delete broken connections
		for (size_t i = 0; i < toErase.size(); i++)
		{
			size_t	pos = toErase[i];
			close(connectedUsers[pos]->getSockfd());
			connectedUsers.erase(connectedUsers.begin() + pos);
			userFDs.erase(userFDs.begin() + pos);
		}
	}
}

void									Server::sendMOTD(const User &user) const
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

int										Server::connectToChannel(const User &user, const std::string &name, const std::string &key)
{
	try
	{
		Channel	*tmp = channels.at(name);
		tmp->connect(user, key);
		return (0);
	}
	catch(const std::exception& e)
	{
		channels[name] = new Channel(name, user, key);
	}
	return (1);
}

void									Server::inviteToChannel(const User &user, const std::string &nickname, const std::string &chanName)
{
	User	*receiver;
	for (size_t i = 0; i < connectedUsers.size(); ++i)
		if (connectedUsers[i]->getNickname() == nickname)
			receiver = connectedUsers[i];
	Channel	*chan = channels.at(chanName);
	if (chan->containsNickname(nickname))
		sendError(user, ERR_USERONCHANNEL, nickname, name);
	else
		chan->invite(user, *receiver);
}
