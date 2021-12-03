#include "Server.hpp"

int		Server::handleChanFlags(const Message &msg, User &user)
{
	std::string	chanName = msg.getParams()[0];
	std::string	flag = msg.getParams()[1];
	if (flag == "+o")
	{
		if (msg.getParams().size() < 3)
			return sendError(user, ERR_NEEDMOREPARAMS, msg.getCommand());
		else if (!containsNickname(msg.getParams()[2]))
			return sendError(user, ERR_NOSUCHNICK, msg.getParams()[2]);
		else
			channels[chanName]->addOperator(*(getUserByName(msg.getParams()[2])));
	}
	else if (flag == "-o")
	{
		if (msg.getParams().size() < 3)
			return sendError(user, ERR_NEEDMOREPARAMS, msg.getCommand());
		else if (!containsNickname(msg.getParams()[2]))
			return sendError(user, ERR_NOSUCHNICK, msg.getParams()[2]);
		else
			channels[chanName]->removeOperator(*(getUserByName(msg.getParams()[2])));
	}
	else if (flag == "+p")
		channels[chanName]->setFlag(PRIVATE);
	else if (flag == "-p")
		channels[chanName]->removeFlag(PRIVATE);
	else if (flag == "+s")
		channels[chanName]->setFlag(SECRET);
	else if (flag == "-s")
		channels[chanName]->removeFlag(SECRET);
	else if (flag == "+i")
		channels[chanName]->setFlag(INVITEONLY);
	else if (flag == "-i")
		channels[chanName]->removeFlag(INVITEONLY);
	else if (flag == "+t")
		channels[chanName]->setFlag(TOPICSET);
	else if (flag == "-t")
		channels[chanName]->removeFlag(TOPICSET);
	else if (flag == "+n")
	{}
	else if (flag == "-n")
	{}
	else if (flag == "+m")
		channels[chanName]->setFlag(MODERATED);
	else if (flag == "-m")
		channels[chanName]->removeFlag(MODERATED);
	else if (flag == "+l")
	{
		if (msg.getParams().size() < 3)
			return sendError(user, ERR_NEEDMOREPARAMS, msg.getCommand());
		else
			channels[chanName]->setLimit(atoi(msg.getParams()[2].c_str()));
	}
	else if (flag == "-l")
	{
		if (msg.getParams().size() < 3)
			return sendError(user, ERR_NEEDMOREPARAMS, msg.getCommand());
		else
			channels[chanName]->setLimit(0);
	}
	else if (flag == "+b")
	{
		if (msg.getParams().size() < 3)
			return sendError(user, ERR_NEEDMOREPARAMS, msg.getCommand());
		else
			channels[chanName]->addBanMask(msg.getParams()[2]);
	}
	else if (flag == "-b")
	{
		if (msg.getParams().size() < 3)
			return sendError(user, ERR_NEEDMOREPARAMS, msg.getCommand());
		else
			channels[chanName]->removeBanMask(msg.getParams()[2]);
	}
	else if (flag == "+v")
	{
		if (msg.getParams().size() < 3)
			return sendError(user, ERR_NEEDMOREPARAMS, msg.getCommand());
		else if (!containsNickname(msg.getParams()[2]))
			return sendError(user, ERR_NOSUCHNICK, msg.getParams()[2]);
		else
			channels[chanName]->addSpeaker(*(getUserByName(msg.getParams()[2])));
	}
	else if (flag == "-v")
	{
		if (msg.getParams().size() < 3)
			return sendError(user, ERR_NEEDMOREPARAMS, msg.getCommand());
		else if (!containsNickname(msg.getParams()[2]))
			return sendError(user, ERR_NOSUCHNICK, msg.getParams()[2]);
		else
			channels[chanName]->removeSpeaker(*(getUserByName(msg.getParams()[2])));
	}
	else if (flag == "+k")
	{
		if (msg.getParams().size() < 3)
			return sendError(user, ERR_NEEDMOREPARAMS, msg.getCommand());
		else
			channels[chanName]->setKey(user, msg.getParams()[2]);
	}
	else if (flag == "-k")
	{
		if (msg.getParams().size() < 3)
			return sendError(user, ERR_NEEDMOREPARAMS, msg.getCommand());
		else
			channels[chanName]->setKey(user, "");
	}
	else
		return sendError(user, ERR_UNKNOWNMODE, flag);
	return 0;
}

int		Server::handleUserFlags(const Message &msg, User &user)
{
	std::string	flag = msg.getParams()[1];
	if (flag == "+i")
		user.setFlag(INVISIBLE);
	else if (flag == "-i")
		user.removeFlag(INVISIBLE);
	else if (flag == "+s")
		user.setFlag(RECEIVENOTICE);
	else if (flag == "-s")
		user.removeFlag(RECEIVENOTICE);
	else if (flag == "+w")
		user.setFlag(RECEIVEWALLOPS);
	else if (flag == "-w")
		user.removeFlag(RECEIVEWALLOPS);
	else if (flag == "+o")
	{}
	else if (flag == "-o")
		user.removeFlag(IRCOPERATOR);
	else
		return sendError(user, ERR_UMODEUNKNOWNFLAG);
	return 0;
}

int		Server::modeCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() < 1)
		sendError(user, ERR_NEEDMOREPARAMS, "MODE");
	else
	{
		if (msg.getParams()[0][0] == '#')
		{
			if (!containsChannel(msg.getParams()[0]))
				sendError(user, ERR_NOSUCHCHANNEL, msg.getParams()[0]);
			else if (!channels.at(msg.getParams()[0])->isOperator(user))
				sendError(user, ERR_CHANOPRIVSNEEDED, msg.getParams()[0]);
			else if (!channels.at(msg.getParams()[0])->containsNickname(user.getNickname()))
				sendError(user, ERR_NOTONCHANNEL, msg.getParams()[0]);
			else if (msg.getParams().size() == 1)
				sendReply(user.getServername(), user, RPL_CHANNELMODEIS, msg.getParams()[0], channels.at(msg.getParams()[0])->getFlagsAsString());
			else if (handleChanFlags(msg, user) != -1)
			{
				std::string	flag = msg.getParams()[1];
				std::string	tmp = (flag[1] == 'o' || flag[1] == 'v') ? " " + msg.getParams()[2] : "";
				channels.at(msg.getParams()[0])->sendMessage("MODE " + msg.getParams()[0] + " " + msg.getParams()[1]  + tmp + "\n", user, true);
			}
		}
		else
		{
			if (msg.getParams()[0] != user.getNickname())
				sendError(user, ERR_USERSDONTMATCH);
			else
			{
				if (msg.getParams().size() == 1)
				{
					std::string	flags = "+";
					if (user.getFlags() & INVISIBLE)
						flags += "i";
					if (user.getFlags() & RECEIVENOTICE)
						flags += "s";
					if (user.getFlags() & RECEIVEWALLOPS)
						flags += "w";
					if (user.getFlags() & IRCOPERATOR)
						flags += "o";
					sendReply(user.getServername(), user, RPL_UMODEIS, flags);
				}
				else if (handleUserFlags(msg, user) != -1)
					user.sendMessage(":" + user.getPrefix() + " MODE " + msg.getParams()[0] + " " + msg.getParams()[1] + "\n");
			}
		}
	}
	return 0;
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

int		Server::joinCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() == 0)
		sendError(user, ERR_NEEDMOREPARAMS, msg.getCommand());
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
				sendError(user, ERR_NOSUCHCHANNEL, chans.front());
			else if (user.getChannels().size() >= maxChannels)
				sendError(user, ERR_TOOMANYCHANNELS, chans.front());
			else if (connectToChannel(user, chans.front(), key) == 1)
				user.addChannel(*(channels.at(chans.front())));
		}
	}
	return 0;
}

int		Server::topicCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() < 1)
		sendError(user, ERR_NEEDMOREPARAMS, "TOPIC");
	else if (!containsChannel(msg.getParams()[0]))
		sendError(user, ERR_NOTONCHANNEL, msg.getParams()[0]);
	else
	{
		Channel	*chan = channels.at(msg.getParams()[0]);
		if (!chan->containsNickname(user.getNickname()))
			sendError(user, ERR_NOTONCHANNEL, msg.getParams()[0]);
		else if (msg.getParams().size() < 2)
			chan->displayTopic(user);
		else
			chan->setTopic(user, msg.getParams()[1]);
	}
	return 0;
}

void	Server::inviteToChannel(const User &user, const std::string &nickname, const std::string &chanName)
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

int		Server::inviteCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() < 2)
		sendError(user, ERR_NEEDMOREPARAMS, "INVITE");
	else if (!containsNickname(msg.getParams()[0]))
		sendError(user, ERR_NOSUCHNICK, msg.getParams()[0]);
	else if (!user.isOnChannel(msg.getParams()[1]) || !containsChannel(msg.getParams()[1]))
		sendError(user, ERR_NOTONCHANNEL, msg.getParams()[1]);
	else
		inviteToChannel(user, msg.getParams()[0], msg.getParams()[1]);
	return 0;
}

int		Server::kickCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() < 2)
		sendError(user, ERR_NEEDMOREPARAMS, "KICK");
	else if (!containsChannel(msg.getParams()[0]))
		sendError(user, ERR_NOSUCHCHANNEL, msg.getParams()[0]);
	else if (!channels.at(msg.getParams()[0])->isOperator(user))
		sendError(user, ERR_CHANOPRIVSNEEDED, msg.getParams()[0]);
	else if (!channels.at(msg.getParams()[0])->containsNickname(user.getNickname()))
		sendError(user, ERR_NOTONCHANNEL, msg.getParams()[0]);
	else if (!containsNickname(msg.getParams()[1]))
		sendError(user, ERR_NOSUCHNICK, msg.getParams()[1]);
	else if (!channels.at(msg.getParams()[0])->containsNickname(msg.getParams()[1]))
		sendError(user, ERR_USERNOTINCHANNEL, msg.getParams()[1], msg.getParams()[0]);
	else
	{
		Channel	*chan = channels.at(msg.getParams()[0]);
		std::string	message = "KICK " + chan->getName() + " " + msg.getParams()[1] + " :";
		if (msg.getParams().size() > 2)
			message += msg.getParams()[2];
		else
			message += user.getNickname();
		chan->sendMessage(message + "\n", user, true);
		chan->disconnect(*(getUserByName(msg.getParams()[1])));
		getUserByName(msg.getParams()[1])->removeChannel(msg.getParams()[0]);
	}
	return 0;
}

int		Server::partCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() < 1)
		sendError(user, ERR_NEEDMOREPARAMS, "PART");
	else
	{
		std::queue<std::string>	chans = split(msg.getParams()[0], ',', false);
		while (chans.size() > 0)
		{
			if (!containsChannel(chans.front()))
				sendError(user, ERR_NOSUCHCHANNEL, chans.front());
			else if (!user.isOnChannel(chans.front()))
				sendError(user, ERR_NOTONCHANNEL, chans.front());
			else
			{
				channels.at(chans.front())->sendMessage("PART " + chans.front() + "\n", user, true);
				channels.at(chans.front())->disconnect(user);
				user.removeChannel(chans.front());
			}
			chans.pop();
		}
	}
	return 0;
}

int		Server::namesCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() == 0)
	{
		std::vector<std::string>	usersWithoutChannel;
		for (size_t i = 0; i < connectedUsers.size(); i++)
			usersWithoutChannel.push_back(connectedUsers[i]->getNickname());
		std::map<std::string, Channel *>::const_iterator	beg = channels.begin();
		std::map<std::string, Channel *>::const_iterator	end = channels.end();
		for (; beg != end; ++beg)
		{
			if (!((*beg).second->getFlags() & SECRET) && !((*beg).second->getFlags() & PRIVATE))
			{
				(*beg).second->displayNames(user);
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
		sendReply(user.getServername(), user, RPL_NAMREPLY, "* *", names);
		sendReply(user.getServername(), user, RPL_ENDOFNAMES, "*");
	}
	else
	{
		std::queue<std::string>	chansToDisplay;
		chansToDisplay = split(msg.getParams()[0], ',', false);
		while (chansToDisplay.size() > 0)
		{
			try
			{
				Channel	*tmp = channels.at(chansToDisplay.front());
				if (!(tmp->getFlags() & SECRET) && !(tmp->getFlags() & PRIVATE))
				{
					tmp->displayNames(user);
					sendReply(user.getServername(), user, RPL_ENDOFNAMES, tmp->getName());
				}
			}
			catch(const std::exception& e)
			{}
			chansToDisplay.pop();
		}
	}
	return 0;
}

int		Server::listCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() > 1 && msg.getParams()[1] != user.getServername())
		return (sendError(user, ERR_NOSUCHSERVER, msg.getParams()[1]));
	std::queue<std::string>	chans;
	std::vector<std::string>	chansToDisplay;
	if (msg.getParams().size() > 0)
	{
		chans = split(msg.getParams()[0], ',', false);
		while (chans.size() > 0)
		{
			if (containsChannel(chans.front()))
				chansToDisplay.push_back(chans.front());
			chans.pop();
		}
	}
	else
	{
		std::map<std::string, Channel *>::const_iterator	beg = channels.begin();
		std::map<std::string, Channel *>::const_iterator	end = channels.end();
		for (; beg != end; ++beg)
			chansToDisplay.push_back((*beg).first);
	}
	sendReply(user.getServername(), user, RPL_LISTSTART);
	for (size_t i = 0; i < chansToDisplay.size(); ++i)
		channels.at(chansToDisplay[i])->displayChanInfo(user);
	sendReply(user.getServername(), user, RPL_LISTEND);
	return 0;
}
