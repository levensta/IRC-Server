#include "Server.hpp"

int 	Server::privmsgCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() == 0)
		return (sendError(user, ERR_NORECIPIENT, msg.getCommand()));
	if (msg.getParams().size() == 1)
		return (sendError(user, ERR_NOTEXTTOSEND));

	std::queue<std::string> receivers = split(msg.getParams()[0], ',', false);
	std::set<std::string> uniqReceivers;

	if (msg.getCommand() == "NOTICE" && (receivers.size() > 1 \
	|| receivers.front()[0] == '#' || receivers.front()[0] == '&'))
		return (sendError(user, ERR_NOSUCHNICK, msg.getParams()[0]));

	while (receivers.size() > 0)
	{
		// checking if there contains dublicate receiver
		if (uniqReceivers.find(receivers.front()) != uniqReceivers.end())
			return (sendError(user, ERR_TOOMANYTARGETS, receivers.front()));
		// if receiver is channel
		if (receivers.front()[0] == '#' || receivers.front()[0] == '&')
		{
			// checking if there such a channel 
			if (!this->containsChannel(receivers.front()))
				return (sendError(user, ERR_NOSUCHNICK, receivers.front()));
			// check that the current user is in the channel
			if (!this->channels[receivers.front()]->containsNickname(user.getNickname()))
				return (sendError(user, ERR_CANNOTSENDTOCHAN, receivers.front()));
		}
		// checking if there such a nickname 
		else if (!this->containsNickname(receivers.front()))
			return (sendError(user, ERR_NOSUCHNICK, msg.getParams()[0]));
		uniqReceivers.insert(receivers.front());
		receivers.pop();
	}
	for (std::set<std::string>::iterator it = uniqReceivers.begin(); it != uniqReceivers.end(); ++it)
	{
		if ((*it)[0] == '#' || (*it)[0] == '&')
		{
			Channel *receiverChannel = this->channels[*it];
			// check that user can send message to channel (user is operator or speaker on moderated channel)
			if (receiverChannel->getFlags() & MODERATED && (!receiverChannel->isOperator(user) && !receiverChannel->isSpeaker(user)))
				sendError(user, ERR_CANNOTSENDTOCHAN, *it);
			else
				receiverChannel->sendMessage(msg.getCommand() + " " + *it + " :" + msg.getParams()[1] + "\n", user, false);
		}
		else
		{
			if (msg.getCommand() == "PRIVMSG" && (this->getUserByName(*it)->getFlags() & AWAY))
				sendReply(user.getServername(), user, RPL_AWAY, *it, this->getUserByName(*it)->getAwayMessage());
			if (msg.getCommand() != "NOTICE" || (this->getUserByName(*it)->getFlags() & RECEIVENOTICE))
				this->getUserByName(*it)->sendMessage(":" + user.getPrefix() + " " + msg.getCommand() + " " + *it + " :" + msg.getParams()[1] + "\n");
		}
	}
	return 0;
}

int		Server::awayCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() == 0)
	{
		user.removeFlag(AWAY);
		sendReply(user.getServername(), user, RPL_UNAWAY);
	}
	else
	{
		user.setFlag(AWAY);
		user.setAwayMessage(msg.getParams()[0]);
		sendReply(user.getServername(), user, RPL_NOWAWAY);
	}
	return 0;
}

int		Server::noticeCmd(const Message &msg, User &user)
{
	privmsgCmd(msg, user);
	return 0;
}

int		Server::whoCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() == 0)
		return (sendError(user, ERR_NEEDMOREPARAMS, msg.getCommand()));

	for (size_t i = 0; i < connectedUsers.size(); ++i)
	{
		if (isEqualToRegex(msg.getParams()[0], connectedUsers[i]->getNickname()) && !(connectedUsers[i]->getFlags() & INVISIBLE))
		{
			std::string channelName = "*";
			std::string userStatus = "";
			const std::vector<const Channel *> userChannels = connectedUsers[i]->getChannels();

			for (int j = userChannels.size() - 1; j >= 0; --j)
			{
				if ((!(userChannels[j]->getFlags() & SECRET) && !(userChannels[j]->getFlags() & PRIVATE)) \
				|| (userChannels[j]->containsNickname(user.getNickname())))
				{
					channelName = userChannels[j]->getName();
					if (userChannels[j]->isOperator(*(connectedUsers[i])))
						userStatus = "@";
					else if (userChannels[j]->isSpeaker(*(connectedUsers[i])))
						userStatus = "+";
					break;
				}
			}

			if (msg.getParams().size() == 1  || msg.getParams()[1] != "o" \
			|| (msg.getParams()[1] == "o" && (connectedUsers[i]->getFlags() & IRCOPERATOR)))
				sendReply(user.getServername(), user, RPL_WHOREPLY, channelName, connectedUsers[i]->getUsername(), connectedUsers[i]->getHostname(), \
							connectedUsers[i]->getServername(), connectedUsers[i]->getNickname(), "H" + userStatus, "0", connectedUsers[i]->getRealname());
		}
	}
	return (sendReply(user.getServername(), user, RPL_ENDOFWHO, user.getNickname()));
}

int		Server::whoisCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() == 0)
		return (sendError(user, ERR_NONICKNAMEGIVEN));
	
	bool suchNick = false;
	for (size_t i = 0; i < connectedUsers.size(); ++i)
	{
		if (isEqualToRegex(msg.getParams()[0], connectedUsers[i]->getNickname()) && !(connectedUsers[i]->getFlags() & IRCOPERATOR))
		{
			sendReply(user.getServername(), user, RPL_WHOISUSER, connectedUsers[i]->getNickname(), \
			connectedUsers[i]->getUsername(), connectedUsers[i]->getHostname(), connectedUsers[i]->getRealname());

			const std::vector<const Channel *> userChannels = connectedUsers[i]->getChannels();
			std::string	channelsList;
			for (size_t j = 0; j < userChannels.size(); ++j)
			{
				if ((!(userChannels[j]->getFlags() & SECRET) && !(userChannels[j]->getFlags() & PRIVATE)) \
				|| (userChannels[j]->containsNickname(user.getNickname())))
				{
					if (j != 0)
						channelsList += " ";
					if (userChannels[j]->isOperator(*(connectedUsers[i])))
						channelsList += "@";
					else if (userChannels[j]->isSpeaker(*(connectedUsers[i])))
						channelsList += "+";
					channelsList += userChannels[j]->getName();
				}
			}
			sendReply(user.getServername(), user, RPL_WHOISCHANNELS, connectedUsers[i]->getNickname(), channelsList);
			sendReply(user.getServername(), user, RPL_WHOISSERVER, connectedUsers[i]->getNickname(), connectedUsers[i]->getServername(), info);
			if (connectedUsers[i]->getFlags() & AWAY)
				sendReply(user.getServername(), user, RPL_AWAY, connectedUsers[i]->getNickname(), connectedUsers[i]->getAwayMessage());
			if (connectedUsers[i]->getFlags() & IRCOPERATOR)
				sendReply(user.getServername(), user, RPL_WHOISOPERATOR, connectedUsers[i]->getNickname());
			std::stringstream	onServer, regTime;
			onServer << (time(0) - connectedUsers[i]->getRegistrationTime());
			regTime << connectedUsers[i]->getRegistrationTime();
			sendReply(user.getServername(), user, RPL_WHOISIDLE, connectedUsers[i]->getNickname(), \
			onServer.str(), regTime.str());
			suchNick = true;
		}
	}
	if (!suchNick)
		sendError(user, ERR_NOSUCHNICK, msg.getParams()[0]);
	return (sendReply(user.getServername(), user, RPL_ENDOFWHOIS, msg.getParams()[0]));
}

int		Server::whowasCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() == 0)
		return (sendError(user, ERR_NONICKNAMEGIVEN));

	else if (!this->containsNickname(msg.getParams()[0]))
	{
		const std::vector<const UserInfo *> historyList = this->nicknamesHistory.getHistoryByName(msg.getParams()[0]);
		if (historyList.size() == 0)
			sendError(user, ERR_WASNOSUCHNICK, msg.getParams()[0]);
		else
		{
			int n = 0;
			if (msg.getParams().size() > 1)
				n = atoi(msg.getParams()[1].c_str());
			n = (n == 0) ? historyList.size() : n;

			for (int i = 0; i < n && i < static_cast<int>(historyList.size()); ++i)
			{
				sendReply(user.getServername(), user, RPL_WHOWASUSER, historyList[i]->getNickname(), \
				historyList[i]->getUsername(), historyList[i]->getHostname(), historyList[i]->getRealname());
				sendReply(user.getServername(), user, RPL_WHOISSERVER, historyList[i]->getNickname(), \
				historyList[i]->getServername(), info); 
			}
		}
	}
	return (sendReply(user.getServername(), user, RPL_ENDOFWHOWAS, msg.getParams()[0]));
}
