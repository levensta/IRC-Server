#include "User.hpp"

User::User(int sockfd, Server &server) :
enterUsername(false), enterNickname(false), registered(false), sockfd(sockfd), role(client)
{
	this->server = &server;
	(void)role;
}

User::~User()
{}

const int					&User::getSockfd() const
{
	return sockfd;
}

const std::string			&User::getNickname() const
{
	return nickname;
}

const std::string			&User::getServername() const
{
	return servername;
}

std::string					User::getPrefix() const
{
	return std::string(nickname + "!" + username + "@" + hostname);
}

void						User::readMessage()
{
	std::string	text;
	if (messages.size() > 0)
		text = messages.front();
	char buffer[100];
	int bytesRead;
	while ((bytesRead = read(sockfd, buffer, 99)) > 0)
	{
		buffer[bytesRead] = 0;
		text += buffer;
		buffer[0] = 0;
		if (text.find('\n'))
			break;
	}
	while (text.find("\r\n") != std::string::npos)
		text.replace(text.find("\r\n"), 2, "\n");
	messages = split(text, '\n', true);
}

void						logMessage(const Message &msg)
{
	std::cout << std::endl << "prefix = " << msg.getPrefix() << ", command = " << msg.getCommand();
	std::cout << ", paramsCount = " << msg.getParams().size() << std::endl;
	const std::vector<std::string>	params = msg.getParams();
	size_t	paramsSize = params.size();
	for (size_t i = 0; i < paramsSize; i++)
	{
		if (i == 0)
			std::cout << "Params list: \"" << params[i] << "\"";
		else
			std::cout << ", \"" << params[i] << "\"";
		if (i == (paramsSize - 1))
			std::cout << std::endl;
	}
	std::cout << std::endl;
}

bool						User::isValidNick(const std::string &nick) const
{
	std::string	special = "-[]\\`^{}";
	for (size_t i = 0; i < nick.size(); i++)
	{
		if ((nick[i] >= 'a' && nick[i] <= 'z')
		|| (nick[i] >= 'A' && nick[i] <= 'Z')
		|| (special.find(nick[i]) != std::string::npos))
			continue ;
		else
			return (false);
	}
	return (true);
}

bool						User::isValidChannelName(const std::string &name) const
{
	if (name[0] != '#' && name[0] != '&')
		return false;
	for (size_t i = 1; i < name.size(); i++)
	{
		if (name[i] == ' ' || name[i] == 7 || name[i] == 0 \
			|| name[i] == 13 || name[i] == 10 || name[i] == ',')
			return false;
	}
	return true;
}

int							User::checkConnection()
{
	if (enterNickname && enterUsername)
	{
		if (password == server->getPassword() || server->getPassword().size() == 0)
		{
			if (!registered)
			{
				registered = true;
				server->sendMOTD(*this);
			}
		}
		else
			return (-1);
	}
	return (0);
}

void						User::passCmd(const Message &msg)
{
	if (msg.getParams().size() == 0)
		sendError(*this, ERR_NEEDMOREPARAMS, "PASS");
	else if (registered)
		sendError(*this, ERR_ALREADYREGISTRED);
	else
		password = msg.getParams()[0];
}

int							User::nickCmd(const Message &msg)
{
	if (msg.getParams().size() == 0)
		sendError(*this, ERR_NEEDMOREPARAMS, "NICK");
	else if (!isValidNick(msg.getParams()[0]))
		sendError(*this, ERR_ERRONEUSNICKNAME, msg.getParams()[0]);
	else if (server->containsNickname(msg.getParams()[0]))
		sendError(*this, ERR_NICKNAMEINUSE, msg.getParams()[0]);
	else
	{
		nickname = msg.getParams()[0];
		enterNickname = true;
	}
	return (checkConnection());
}

int							User::userCmd(const Message &msg)
{
	if (msg.getParams().size() < 4)
		sendError(*this, ERR_NEEDMOREPARAMS, "USER");
	else if (registered)
		sendError(*this, ERR_ALREADYREGISTRED);
	else
	{ // Проверить валидность всего этого
		username = msg.getParams()[0];
		hostname = msg.getParams()[1];
		servername = msg.getParams()[2];
		realname = msg.getParams()[3];
		enterUsername = true;
	}
	return (checkConnection());
}

void 						User::privmsgCmd(const Message &msg)
{
	if (msg.getParams().size() == 0)
		sendError(*this, ERR_NORECIPIENT, "PRIVMSG");
	else if (msg.getParams().size() == 1)
		sendError(*this, ERR_NOTEXTTOSEND);
	else
	{
		std::queue<std::string> receivers = split(msg.getParams()[0], ',', false);
		std::set<std::string> uniqReceivers;
		while (receivers.size() > 0)
		{
			if (uniqReceivers.find(receivers.front()) != uniqReceivers.end())
			{
				sendError(*this, ERR_TOOMANYTARGETS, receivers.front());
				return ;
			}
//			if (receivers.front()[0] == '#' || receivers.front()[0] == '&')
//			{
//				check channel;
//			}
			else if (!this->server->containsNickname(receivers.front()))
			{
				sendError(*this, ERR_NORECIPIENT, "PRIVMSG");
				return ;
			}
			uniqReceivers.insert(receivers.front());
			receivers.pop();
		}
		for (std::set<std::string>::iterator it = uniqReceivers.begin(); it != uniqReceivers.end(); ++it)
		{
//			if (*it[0] == '#' || *it[0] == '&')
//			{
//				sendMessage to channel
//				return ;
//			}
//			else
//			{
				std::vector<User *> users = this->server->getConnectedUsers();
				for (size_t i = 0; i < users.size(); ++i)
				{
					if (users[i]->getNickname() == *it)
					{
						users[i]->sendMessage(msg.getParams()[1] + "\n");
						break;
					}
				}
//			}
		}
	}
}

void						User::joinCmd(const Message &msg)
{
	if (msg.getParams().size() == 0)
		sendError(*this, ERR_NEEDMOREPARAMS, "JOIN");
	else
	{
		std::queue<std::string>	chans = split(msg.getParams()[0], ',', false);
		std::queue<std::string>	keys;
		if (msg.getParams().size() > 1)
			keys = split(msg.getParams()[1], ',', false);
		for (; chans.size() > 0; chans.pop())
		{
			std::string	key = keys.size() ? keys.front() : "";
			if (keys.size() > 0)
				keys.pop();
			if (!isValidChannelName(chans.front()))
			{
				sendError(*this, ERR_NOSUCHCHANNEL, chans.front());
				continue ;
			}
			else if (channels.size() == 10) // add maxsize to config
			{
				sendError(*this, ERR_TOOMANYCHANNELS, chans.front());
				continue ;
			}
			else if (server->connectToChannel(*this, chans.front(), key) == 1)
				channels.push_back(server->getChannels().at(chans.front()));
		}
	}
}

int							User::hadleMessages()
{
	while (messages.size() > 0 && messages.front().back() == '\n')
	{
		Message	msg(messages.front());
		messages.pop();
		// log message to server console
		logMessage(msg);
		// handle
		if (msg.getCommand() == "QUIT")
			return (-1);
		else if (!registered)
		{
			if (msg.getCommand() == "PASS")
				this->passCmd(msg);
			else if (msg.getCommand() == "USER")
			{
				if (this->userCmd(msg) == -1)
					return (-1);
			}
			else if (msg.getCommand() == "NICK")
			{
				if (this->nickCmd(msg) == -1)
					return (-1);
			}
			else
				sendError(*this, ERR_NOTREGISTERED);
		}
		else
		{
			if (msg.getCommand() == "JOIN")
				this->joinCmd(msg);
			else if (msg.getCommand() == "PRIVMSG")
				this->privmsgCmd(msg);
		}
	}
	return (0);
}

void						User::sendMessage(const std::string &msg) const
{
	if (msg.size() > 0)
		send(sockfd, msg.c_str(), msg.size(), 0);
}
