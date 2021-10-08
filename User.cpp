#include "User.hpp"

User::User(int sockfd, Server &server) :
enterUsername(false), enterNickname(false), registered(false), away(false),
invisible(false), receiveNotice(true), receiveWallops(true), role(client), sockfd(sockfd)
{
	this->server = &server;
	(void)role;
}

User::~User()
{}

int							User::getSockfd() const
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

const std::string			&User::getAwayMessage() const
{
	return (awayMessage);
}

bool						User::isAway() const
{
	return away;
}

bool						User::isInvisible() const
{
	return invisible;
}

bool						User::isReceiveNotice() const
{
	return receiveNotice;
}

bool						User::isReceiveWallops() const
{
	return receiveNotice;
}

Channel						*User::getChanByName(const std::string &name)
{
	Channel	*ret;
	for (size_t i = 0; i < channels.size(); ++i)
		if (channels[i]->getName() == name)
			ret = channels[i];
	return ret;
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
		if (text.find('\n') != std::string::npos)
			break;
	}
	while (text.find("\r\n") != std::string::npos)
		text.replace(text.find("\r\n"), 2, "\n");
	if (text.size() > 1)
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

bool						User::isOnChannel(const std::string &name) const
{
	for (size_t i = 0; i < channels.size(); i++)
		if (channels[i]->getName() == name)
			return true;
	return false;
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
		sendError(*this, ERR_NORECIPIENT, msg.getCommand());
	else if (msg.getParams().size() == 1)
		sendError(*this, ERR_NOTEXTTOSEND);
	else
	{
		std::queue<std::string> receivers = split(msg.getParams()[0], ',', false);
		std::set<std::string> uniqReceivers;
		if (msg.getCommand() == "NOTICE" && (receivers.size() > 1 \
		|| receivers.front()[0] == '#' || receivers.front()[0] == '&'))
		{
			sendError(*this, ERR_NOSUCHNICK, msg.getParams()[0]);
			return ;
		}
		while (receivers.size() > 0)
		{
			// checking if there contains dublicate receiver
			if (uniqReceivers.find(receivers.front()) != uniqReceivers.end())
			{
				sendError(*this, ERR_TOOMANYTARGETS, receivers.front());
				return ;
			}
			// if receiver is channel
			if (receivers.front()[0] == '#' || receivers.front()[0] == '&')
			{
				// checking if there such a channel 
				if (!this->server->containsChannel(receivers.front()))
				{
					sendError(*this, ERR_NOSUCHNICK, receivers.front());
					return ;
				}
				// check that the current user is in the channel
				if (this->server->getChannels()[receivers.front()]->containsNickname(this->nickname))
				{
					sendError(*this, ERR_CANNOTSENDTOCHAN, receivers.front());
					return ;
				}
			}
			// checking if there such a nickname 
			else if (!this->server->containsNickname(receivers.front()))
			{
				sendError(*this, ERR_NOSUCHNICK, msg.getParams()[0]);
				return ;
			}
			uniqReceivers.insert(receivers.front());
			receivers.pop();
		}
		for (std::set<std::string>::iterator it = uniqReceivers.begin(); it != uniqReceivers.end(); ++it)
		{
			if ((*it)[0] == '#' || (*it)[0] == '&')
			{
				Channel *receiverChannel = this->server->getChannels()[*it];
				// check that user can send message to channel (user is operator or speaker on moderated channel)
				if (receiverChannel->getFlags() & MODERATED && (!receiverChannel->isOperator(*this) && !receiverChannel->isSpeaker(*this)))
					sendError(*this, ERR_CANNOTSENDTOCHAN, *it);
				else
					receiverChannel->sendMessage(msg.getParams()[1] + "\n", *this);
			}
			else
			{
				if (msg.getCommand() == "PRIVMSG" && this->server->getUserByName(*it)->isAway())
					sendReply(this->servername, *this, RPL_AWAY, *it, this->server->getUserByName(*it)->getAwayMessage()); 
				this->server->getUserByName(*it)->sendMessage(":" + getPrefix() + " " + msg.getCommand() + " " + *it + " :" + msg.getParams()[1] + "\n");
			}
		}
	}
}

void						User::awayCmd(const Message &msg)
{
	if (msg.getParams().size() == 0)
	{
		this->away = false;
		sendReply(this->servername, *this, RPL_UNAWAY);
	}
	else
	{
		this->away = true;
		this->awayMessage = msg.getParams()[0];
		sendReply(this->servername, *this, RPL_NOWAWAY);
	}
}

void						User::noticeCmd(const Message &msg)
{
	this->privmsgCmd(msg);
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
				sendError(*this, ERR_NOSUCHCHANNEL, chans.front());
			else if (channels.size() == 10) // add maxsize to config
				sendError(*this, ERR_TOOMANYCHANNELS, chans.front());
			else if (server->connectToChannel(*this, chans.front(), key) == 1)
				channels.push_back(server->getChannels().at(chans.front()));
		}
	}
}

void						User::inviteCmd(const Message &msg)
{
	if (msg.getParams().size() < 2)
		sendError(*this, ERR_NEEDMOREPARAMS, "INVITE");
	else if (!server->containsNickname(msg.getParams()[0]))
		sendError(*this, ERR_NOSUCHNICK, msg.getParams()[0]);
	else if (!isOnChannel(msg.getParams()[1]) || !server->containsChannel(msg.getParams()[1]))
		sendError(*this, ERR_NOTONCHANNEL, msg.getParams()[1]);
	else
		server->inviteToChannel(*this, msg.getParams()[0], msg.getParams()[1]);
}

int							User::handleChanFlags(const Message &msg)
{
	std::string	chanName = msg.getParams()[0];
	std::string	flag = msg.getParams()[1];
	if (flag == "+o")
	{
		if (msg.getParams().size() < 3)
			return sendError(*this, ERR_NEEDMOREPARAMS, msg.getCommand());
		else if (!server->containsNickname(msg.getParams()[2]))
			return sendError(*this, ERR_NOSUCHNICK, msg.getParams()[2]);
		else
			getChanByName(chanName)->addOperator(*(server->getUserByName(msg.getParams()[2])));
	}
	else if (flag == "-o")
	{
		if (msg.getParams().size() < 3)
			return sendError(*this, ERR_NEEDMOREPARAMS, msg.getCommand());
		else if (!server->containsNickname(msg.getParams()[2]))
			return sendError(*this, ERR_NOSUCHNICK, msg.getParams()[2]);
		else
			getChanByName(chanName)->removeOperator(*(server->getUserByName(msg.getParams()[2])));
	}
	else if (flag == "+p")
		getChanByName(chanName)->setFlag(PRIVATE);
	else if (flag == "-p")
		getChanByName(chanName)->removeFlag(PRIVATE);
	else if (flag == "+s")
		getChanByName(chanName)->setFlag(SECRET);
	else if (flag == "-s")
		getChanByName(chanName)->removeFlag(SECRET);
	else if (flag == "+i")
		getChanByName(chanName)->setFlag(INVITEONLY);
	else if (flag == "-i")
		getChanByName(chanName)->removeFlag(INVITEONLY);
	else if (flag == "+t")
		getChanByName(chanName)->setFlag(TOPICSET);
	else if (flag == "-t")
		getChanByName(chanName)->removeFlag(TOPICSET);
	else if (flag == "+n")
	{}
	else if (flag == "-n")
	{}
	else if (flag == "+m")
		getChanByName(chanName)->setFlag(MODERATED);
	else if (flag == "-m")
		getChanByName(chanName)->removeFlag(MODERATED);
	else if (flag == "+l")
	{
		if (msg.getParams().size() < 3)
			return sendError(*this, ERR_NEEDMOREPARAMS, msg.getCommand());
		else
			getChanByName(chanName)->setLimit(atoi(msg.getParams()[2].c_str()));
	}
	else if (flag == "-l")
	{
		if (msg.getParams().size() < 3)
			return sendError(*this, ERR_NEEDMOREPARAMS, msg.getCommand());
		else
			getChanByName(chanName)->setLimit(0);
	}
	else if (flag == "+b")
	{
		if (msg.getParams().size() < 3)
			return sendError(*this, ERR_NEEDMOREPARAMS, msg.getCommand());
		else
			getChanByName(chanName)->addBanMask(msg.getParams()[2]);
	}
	else if (flag == "-b")
	{
		if (msg.getParams().size() < 3)
			return sendError(*this, ERR_NEEDMOREPARAMS, msg.getCommand());
		else
			getChanByName(chanName)->removeBanMask(msg.getParams()[2]);
	}
	else if (flag == "+v")
	{
		if (msg.getParams().size() < 3)
			return sendError(*this, ERR_NEEDMOREPARAMS, msg.getCommand());
		else if (!server->containsNickname(msg.getParams()[2]))
			return sendError(*this, ERR_NOSUCHNICK, msg.getParams()[2]);
		else
			getChanByName(chanName)->addSpeaker(*(server->getUserByName(msg.getParams()[2])));
	}
	else if (flag == "-v")
	{
		if (msg.getParams().size() < 3)
			return sendError(*this, ERR_NEEDMOREPARAMS, msg.getCommand());
		else if (!server->containsNickname(msg.getParams()[2]))
			return sendError(*this, ERR_NOSUCHNICK, msg.getParams()[2]);
		else
			getChanByName(chanName)->removeSpeaker(*(server->getUserByName(msg.getParams()[2])));
	}
	else if (flag == "+k")
	{
		if (msg.getParams().size() < 3)
			return sendError(*this, ERR_NEEDMOREPARAMS, msg.getCommand());
		else
			getChanByName(chanName)->setKey(*this, msg.getParams()[2]);
	}
	else if (flag == "-k")
	{
		if (msg.getParams().size() < 3)
			return sendError(*this, ERR_NEEDMOREPARAMS, msg.getCommand());
		else
			getChanByName(chanName)->setKey(*this, "");
	}
	else
		return sendError(*this, ERR_UNKNOWNMODE, flag);
	return 0;
}

int							User::handleUserFlags(const Message &msg)
{
	std::string	flag = msg.getParams()[1];
	if (flag == "+i")
		invisible = true;
	else if (flag == "-i")
		invisible = false;
	else if (flag == "+s")
		receiveNotice = true;
	else if (flag == "-s")
		receiveNotice = false;
	else if (flag == "+w")
		receiveWallops = true;
	else if (flag == "-w")
		receiveWallops = false;
	else if (flag == "+o")
	{}
	else if (flag == "-o")
		role = client;
	else
		return sendError(*this, ERR_UMODEUNKNOWNFLAG);
	return 0;
}

void						User::modeCmd(const Message &msg)
{
	if (msg.getParams().size() < 1)
		sendError(*this, ERR_NEEDMOREPARAMS, "MODE");
	else
	{
		if (msg.getParams()[0][0] == '#')
		{
			if (!server->containsChannel(msg.getParams()[0]))
				sendError(*this, ERR_NOSUCHCHANNEL, msg.getParams()[0]);
			else if (!server->getChannels().at(msg.getParams()[0])->isOperator(*this))
				sendError(*this, ERR_CHANOPRIVSNEEDED, msg.getParams()[0]);
			else if (!server->getChannels().at(msg.getParams()[0])->containsNickname(nickname))
				sendError(*this, ERR_NOTONCHANNEL, msg.getParams()[0]);
			else if (msg.getParams().size() == 1)
			{
				Channel	*chan = server->getChannels().at(msg.getParams()[0]);
				std::string	flags;
				if (chan->getFlags() & INVITEONLY)
					flags += "i";
				if (chan->getFlags() & NOMSGOUT)
					flags += "n";
				if (chan->getFlags() & PRIVATE)
					flags += "p";
				if (chan->getFlags() & SECRET)
					flags += "s";
				if (chan->getFlags() & TOPICSET)
					flags += "t";
				sendReply(servername, *this, RPL_CHANNELMODEIS, msg.getParams()[0], flags);
			}
			else if (handleChanFlags(msg) != -1)
				server->getChannels().at(msg.getParams()[0])->sendMessage("MODE " + msg.getParams()[0] + " " + msg.getParams()[1] + "\n", *this);
		}
		else
		{
			if (msg.getParams()[0] != nickname)
				sendError(*this, ERR_USERSDONTMATCH);
			else
			{
				if (msg.getParams().size() == 1)
				{
					std::string	flags = "+";
					if (invisible)
						flags += "i";
					if (receiveNotice)
						flags += "s";
					if (receiveWallops)
						flags += "w";
					if (role == IrcOperator)
						flags += "o";
					sendReply(servername, *this, RPL_UMODEIS, flags);
				}
				else if (handleUserFlags(msg) != -1)
					sendMessage(":" + getPrefix() + "MODE " + msg.getParams()[0] + " " + msg.getParams()[1] + "\n");
			}
		}
	}
}

void						User::topicCmd(const Message &msg)
{
	if (msg.getParams().size() < 1)
		sendError(*this, ERR_NEEDMOREPARAMS, "TOPIC");
	else if (!server->containsChannel(msg.getParams()[0]))
		sendError(*this, ERR_NOTONCHANNEL, msg.getParams()[0]);
	else
	{
		Channel	*chan = server->getChannels().at(msg.getParams()[0]);
		if (!chan->containsNickname(nickname))
			sendError(*this, ERR_NOTONCHANNEL, msg.getParams()[0]);
		else if (msg.getParams().size() < 2)
			chan->displayTopic(*this);
		else
			chan->setTopic(*this, msg.getParams()[1]);
	}
}

void						User::namesCmd(const Message &msg)
{
	std::map<std::string, Channel *>	chans = server->getChannels();
	if (msg.getParams().size() == 0)
	{
		std::vector<std::string>	usersWithoutChannel;
		const std::vector<User *>	users = server->getUsers();
		for (size_t i = 0; i < users.size(); i++)
			usersWithoutChannel.push_back(users[i]->getNickname());
		std::map<std::string, Channel *>::const_iterator	beg = chans.begin();
		std::map<std::string, Channel *>::const_iterator	end = chans.end();
		for (; beg != end; ++beg)
		{
			if (!((*beg).second->getFlags() & SECRET) && !((*beg).second->getFlags() & PRIVATE))
			{
				(*beg).second->displayNames(*this);
				for (size_t i = 0; i < usersWithoutChannel.size(); i++)
					if ((*beg).second->containsNickname(usersWithoutChannel[i]))
						usersWithoutChannel.erase(usersWithoutChannel.begin() + i--);
			}
		}
		std::string	names;
		for (size_t i = 0; i < usersWithoutChannel.size(); i++)
		{
			names += usersWithoutChannel[i];
			if (i != (usersWithoutChannel.size() - 1))
				names += " ";
		}
		sendReply(servername, *this, RPL_NAMREPLY, "* *", names);
		sendReply(servername, *this, RPL_ENDOFNAMES, "*");
	}
	else
	{
		std::queue<std::string>	chansToDisplay;
		chansToDisplay = split(msg.getParams()[0], ',', false);
		while (chansToDisplay.size() > 0)
		{
			try
			{
				Channel	*tmp = chans.at(chansToDisplay.front());
				if (!(tmp->getFlags() & SECRET) && !(tmp->getFlags() & PRIVATE))
				{
					tmp->displayNames(*this);
					sendReply(servername, *this, RPL_ENDOFNAMES, tmp->getName());
				}
			}
			catch(const std::exception& e)
			{}
			chansToDisplay.pop();
		}
	}
}

void						User::kickCmd(const Message &msg)
{
	if (msg.getParams().size() < 2)
		sendError(*this, ERR_NEEDMOREPARAMS, "KICK");
	else if (!server->containsChannel(msg.getParams()[0]))
		sendError(*this, ERR_NOSUCHCHANNEL, msg.getParams()[0]);
	else if (!server->getChannels().at(msg.getParams()[0])->isOperator(*this))
		sendError(*this, ERR_CHANOPRIVSNEEDED, msg.getParams()[0]);
	else if (!server->getChannels().at(msg.getParams()[0])->containsNickname(nickname))
		sendError(*this, ERR_NOTONCHANNEL, msg.getParams()[0]);
	else if (!server->containsNickname(msg.getParams()[1]))
		sendError(*this, ERR_NOSUCHNICK, msg.getParams()[1]);
	else
	{
		Channel	*chan = server->getChannels().at(msg.getParams()[0]);
		std::string	message = "KICK " + chan->getName() + " " + msg.getParams()[1] + " :";
		if (msg.getParams().size() > 2)
			message += msg.getParams()[2];
		else
			message += nickname;
		chan->sendMessage(message + "\n", *this);
		chan->disconnect(*(server->getUserByName(msg.getParams()[1])));
		server->getUserByName(msg.getParams()[1])->removeChannel(msg.getParams()[0]);
	}
}

int							User::partCmd(const Message &msg)
{
	if (msg.getParams().size() < 1)
		sendError(*this, ERR_NEEDMOREPARAMS, "PART");
	else
	{
		std::queue<std::string>	chans = split(msg.getParams()[0], ',', false);
		std::vector<std::string>	chansInVector;
		while (chans.size() > 0)
		{
			if (!server->containsChannel(chans.front()))
				return sendError(*this, ERR_NOSUCHCHANNEL, chans.front());
			if (!isOnChannel(chans.front()))
				return sendError(*this, ERR_NOTONCHANNEL, chans.front());
			chansInVector.push_back(chans.front());
			chans.pop();
		}
		for (size_t i = 0; i < chansInVector.size(); ++i)
		{
			server->getChannels().at(chansInVector[i])->sendMessage("PART " + chansInVector[i] + "\n", *this);
			server->getChannels().at(chansInVector[i])->disconnect(*this);
			removeChannel(chansInVector[i]);
		}
	}
	return 0;
}

int							User::hadleMessages()
{
	while (messages.size() > 0 && messages.front()[messages.front().size() - 1] == '\n')
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
			std::cout << nickname << " in " << channels.size() << "chans\n";
			if (msg.getCommand() == "JOIN")
				this->joinCmd(msg);
			else if (msg.getCommand() == "PRIVMSG")
				this->privmsgCmd(msg);
			else if (msg.getCommand() == "AWAY")
				this->awayCmd(msg);
			else if (msg.getCommand() == "NOTICE")
				this->noticeCmd(msg);
			else if (msg.getCommand() == "INVITE")
				this->inviteCmd(msg);
			else if (msg.getCommand() == "MODE")
				this->modeCmd(msg);
			else if (msg.getCommand() == "TOPIC")
				this->topicCmd(msg);
			else if (msg.getCommand() == "NAMES")
				this->namesCmd(msg);
			else if (msg.getCommand() == "KICK")
				this->kickCmd(msg);
			else if (msg.getCommand() == "PART")
				this->partCmd(msg);
		}
	}
	return (0);
}

void						User::sendMessage(const std::string &msg) const
{
	if (msg.size() > 0)
		send(sockfd, msg.c_str(), msg.size(), 0);
}

void						User::removeChannel(const std::string &name)
{
	std::vector<Channel *>::iterator	begin = channels.begin();
	std::vector<Channel *>::iterator	end = channels.end();
	for (; begin != end; ++begin)
		if ((*begin)->getName() == name)
			break ;
	channels.erase(begin);
}
