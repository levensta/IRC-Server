#include "Channel.hpp"

Channel::Channel(const std::string &name, const User &creator, const std::string &pass) :
name(name), pass(pass), userLimit(0), flags(NOMSGOUT)
{
	users.push_back(&creator);
	operators.push_back(&creator);
	sendInfo(creator);
}

Channel::~Channel()
{}

void	Channel::sendInfo(const User &user)
{
	sendMessage("JOIN :" + name + "\n", user, true);
	displayTopic(user);
	displayNames(user);
	sendReply(user.getServername(), user, RPL_ENDOFNAMES, name);
}

void	Channel::displayTopic(const User &user)
{
	if (topic.size() > 0)
		sendReply(user.getServername(), user, RPL_TOPIC, name, topic);
	else
		sendReply(user.getServername(), user, RPL_NOTOPIC, name);
}

void	Channel::displayNames(const User &user)
{
	std::string	names;
	std::vector<const User *>::const_iterator	beg = users.begin();
	std::vector<const User *>::const_iterator	end = users.end();
	while (beg != end)
	{
		const User	*tmp = *beg;
		if (isOperator(*tmp))
			names += "@";
		else if (isSpeaker(*tmp))
			names += "+";
		names += tmp->getNickname();
		++beg;
		if (beg != end)
			names += " ";
	}
	sendReply(user.getServername(), user, RPL_NAMREPLY, "= " + name, names);
}

const std::string	&Channel::getName() const
{
	return (name);
}

const std::string	&Channel::getTopic() const
{
	return (topic);
}

void	Channel::setTopic(const User &user, const std::string &topic)
{
	if ((flags & TOPICSET) && !isOperator(user))
		sendError(user, ERR_CHANOPRIVSNEEDED, name);
	else
	{
		this->topic = topic;
		sendMessage("TOPIC " + name + " :" + this->topic + "\n", user, true);
	}
}

void	Channel::setKey(const User &user, const std::string &key)
{
	if (pass.size() > 0 && key.size() > 0)
		sendError(user, ERR_KEYSET, name);
	else
		this->pass = key;
}

unsigned char	Channel::getFlags() const
{
	return flags;
}

bool	Channel::isBanned(const std::string &mask, const std::string &prefix)
{
	return (isEqualToRegex(mask, prefix));
}

bool	Channel::isInvited(const User &user) const
{
	for (size_t i = 0; i < invitedUsers.size(); i++)
		if (invitedUsers[i]->getPrefix() == user.getPrefix())
			return true;
	return false;
}

bool	Channel::isOperator(const User &user) const
{
	for (size_t i = 0; i < operators.size(); i++)
		if (operators[i]->getPrefix() == user.getPrefix())
			return true;
	return false;
}

bool	Channel::isSpeaker(const User &user) const
{
	for (size_t i = 0; i < speakers.size(); i++)
		if (speakers[i]->getPrefix() == user.getPrefix())
			return true;
	return false;
}

bool	Channel::isEmpty() const
{
	if (users.size() == 0)
		return true;
	return false;
}

bool	Channel::containsNickname(const std::string &nickname) const
{
	std::vector<const User *>::const_iterator	beg = users.begin();
	std::vector<const User *>::const_iterator	end = users.end();
	for (; beg != end; ++beg)
		if ((*beg)->getNickname() == nickname)
			return (true);
	return (false);
}

void	Channel::connect(const User &user, const std::string &key)
{
	if ((flags & PRIVATE) && key != pass)
		sendError(user, ERR_BADCHANNELKEY, name);
	else if (userLimit != 0 && users.size() >= userLimit)
		sendError(user, ERR_CHANNELISFULL, name);
	else if ((flags & INVITEONLY) && !isInvited(user))
		sendError(user, ERR_INVITEONLYCHAN, name);
	else
	{
		for (size_t i = 0; i < banMasks.size(); i++)
		{
			if (isBanned(banMasks[i], user.getPrefix()))
			{
				sendError(user, ERR_BANNEDFROMCHAN, name);
				return ;
			}
		}
		std::vector<const User *>::iterator	begin = users.begin();
		std::vector<const User *>::iterator	end = users.end();
		for (; begin != end; ++begin)
			if ((*begin)->getPrefix() == user.getPrefix())
				return ;
		users.push_back(&user);
		removeInvited(user);
		sendInfo(user);
	}
}

void	Channel::setFlag(unsigned char flag)
{
	flags |= flag;
}

void	Channel::removeFlag(unsigned char flag)
{
	flags &= ~flag;
}

void	Channel::sendMessage(const std::string &message, const User &from, bool includeUser) const
{
	std::string	msg;
	msg += ":" + from.getPrefix() + " " + message;
	std::vector<const User *>::const_iterator	begin = users.begin();
	std::vector<const User *>::const_iterator	end = users.end();
	for (; begin != end; ++begin)
	{
		if (includeUser || *begin != &from)
			(*begin)->sendMessage(msg);
	}
}

void	Channel::invite(const User &user, const User &receiver)
{
	if (flags & INVITEONLY && !isOperator(user))
		sendError(user, ERR_CHANOPRIVSNEEDED, name);
	else
	{
		invitedUsers.push_back(&receiver);
		receiver.sendMessage(":" + user.getPrefix() + " INVITE " + receiver.getNickname() + " :" + name + "\n");
		sendReply(user.getServername(), user, RPL_INVITING, name, receiver.getNickname());
		if (receiver.getFlags() & AWAY)
			sendReply(user.getServername(), user, RPL_AWAY, receiver.getNickname(), receiver.getAwayMessage());
	}
}

void	Channel::addOperator(const User &user)
{
	if (!isOperator(user))
		operators.push_back(&user);
}

void	Channel::removeOperator(const User &user)
{
	if (isOperator(user))
	{
		size_t	i;
		for (i = 0; i < operators.size(); i++)
			if (operators[i] == &user)
				break;
		operators.erase(operators.begin() + i);
		if (operators.size() == 0 && users.size() > 0)
		{
			operators.push_back(users[0]);
			sendMessage("MODE " + this->name + " +o "  + users[0]->getNickname() + "\n", user, true);
		}
	}
}

void	Channel::addSpeaker(const User &user)
{
	if (!isSpeaker(user))
		speakers.push_back(&user);
}

void	Channel::removeSpeaker(const User &user)
{
	if (isSpeaker(user))
	{
		size_t	i;
		for (i = 0; i < speakers.size(); i++)
			if (speakers[i] == &user)
				break;
		speakers.erase(speakers.begin() + i);
	}
}

void	Channel::setLimit(unsigned short limit)
{
	userLimit = limit;
}

void	Channel::addBanMask(const std::string &mask)
{
	banMasks.push_back(mask);
}

void	Channel::removeBanMask(const std::string &mask)
{
	size_t	i;
	for (i = 0; i < banMasks.size(); i++)
		if (banMasks[i] == mask)
			break;
	banMasks.erase(banMasks.begin() + i);
	
}

void	Channel::disconnect(const User &user)
{
	std::vector<const User *>::iterator	begin = users.begin();
	std::vector<const User *>::iterator	end = users.end();
	for (; begin != end; ++begin)
		if (*begin == &user)
			break ;
	users.erase(begin);
	removeOperator(user);
	removeSpeaker(user);
}

void	Channel::removeInvited(const User &user)
{
	if (isInvited(user))
	{
		size_t	i;
		for (i = 0; i < invitedUsers.size(); i++)
			if (invitedUsers[i] == &user)
				break;
		invitedUsers.erase(invitedUsers.begin() + i);
	}
}

std::string	Channel::getFlagsAsString() const
{
	std::string	ret;
	if (flags & INVITEONLY)
		ret += "i";
	if (flags & NOMSGOUT)
		ret += "n";
	if (flags & PRIVATE)
		ret += "p";
	if (flags & SECRET)
		ret += "s";
	if (flags & TOPICSET)
		ret += "t";
	return ret;
}

void	Channel::displayChanInfo(const User &user)
{
	std::string	chanName = "";
	std::string	info = "";
	if ((flags & SECRET) && !containsNickname(user.getNickname()))
		return ;
	if ((flags & PRIVATE) && !containsNickname(user.getNickname()))
		chanName = "*";
	else
	{
		chanName = name;
		info = "[+" + getFlagsAsString() + "] " + topic;
	}
	std::stringstream	ss;
	ss << users.size();
	sendReply(user.getServername(), user, RPL_LIST, chanName, ss.str(), info);
}
