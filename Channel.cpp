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

void				Channel::sendInfo(const User &user)
{
	sendMessage("JOIN :" + name + "\n", user);
	displayTopic(user);
	displayNames(user);
	sendReply(user.getServername(), user, RPL_ENDOFNAMES, name);
}

void				Channel::displayTopic(const User &user)
{
	if (topic.size() > 0)
		sendReply(user.getServername(), user, RPL_TOPIC, name, topic);
	else
		sendReply(user.getServername(), user, RPL_NOTOPIC, name);
}

void				Channel::displayNames(const User &user)
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

void				Channel::setTopic(const User &user, const std::string &topic)
{
	if ((flags & TOPICSET) && !isOperator(user))
		sendError(user, ERR_CHANOPRIVSNEEDED, name);
	else
	{
		this->topic = topic;
		sendMessage("TOPIC " + name + " :" + this->topic + "\n", user);
	}
}

void				Channel::setKey(const User &user, const std::string &key)
{
	if (pass.size() > 0 && key.size() > 0)
		sendError(user, ERR_KEYSET, name);
	else
		this->pass = key;
}

unsigned char		&Channel::getFlags()
{
	return flags;
}

bool				Channel::isBanned(const std::string &mask, const std::string &prefix)
{
	const char *rs=0, *rp;
	const char *s = prefix.c_str();
	const char *p = mask.c_str();
	while (1)
	{
		if (*p == '*')
		{
			rs = s;
			rp = ++p;
		}
		else if (!*s)
		{
			return (!(*p));
		}
		else if (*s == *p)
		{
			++s;
			++p;
		}
		else if (rs)
		{
			s = ++rs;
			p = rp;
		}
		else
		{
			return (false);
		}
	}
}

bool				Channel::isInvited(const User &user) const
{
	for (size_t i = 0; i < invitedUsers.size(); i++)
		if (invitedUsers[i]->getPrefix() == user.getPrefix())
			return true;
	return false;
}

bool				Channel::isOperator(const User &user) const
{
	for (size_t i = 0; i < operators.size(); i++)
		if (operators[i]->getPrefix() == user.getPrefix())
			return true;
	return false;
}

bool				Channel::isSpeaker(const User &user) const
{
	for (size_t i = 0; i < speakers.size(); i++)
		if (speakers[i]->getPrefix() == user.getPrefix())
			return true;
	return false;
}

bool				Channel::containsNickname(const std::string &nickname) const
{
	std::vector<const User *>::const_iterator	beg = users.begin();
	std::vector<const User *>::const_iterator	end = users.end();
	for (; beg != end; ++beg)
		if ((*beg)->getNickname() == nickname)
			return (true);
	return (false);
}

void				Channel::connect(const User &user, const std::string &key)
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

void				Channel::setFlag(unsigned char flag)
{
	flags |= flag;
}

void				Channel::removeFlag(unsigned char flag)
{
	flags &= ~flag;
}

void				Channel::sendMessage(const std::string &message, const User &from)
{
	std::string	msg;
	msg += ":" + from.getPrefix() + " " + message;
	std::vector<const User *>::iterator	begin = users.begin();
	std::vector<const User *>::iterator	end = users.end();
	for (; begin != end; ++begin)
		(*begin)->sendMessage(msg);
}

void				Channel::invite(const User &user, const User &receiver)
{
	if (flags & INVITEONLY && !isOperator(user))
		sendError(user, ERR_CHANOPRIVSNEEDED, name);
	else
	{
		invitedUsers.push_back(&receiver);
		receiver.sendMessage(":" + user.getPrefix() + " INVITE " + receiver.getNickname() + " :" + name + "\n");
		sendReply(user.getServername(), user, RPL_INVITING, name, receiver.getNickname());
		if (receiver.isAway())
			sendReply(user.getServername(), user, RPL_AWAY, receiver.getNickname(), receiver.getAwayMessage());
	}
}

void				Channel::addOperator(const User &user)
{
	if (!isOperator(user))
		operators.push_back(&user);
}

void				Channel::removeOperator(const User &user)
{
	if (isOperator(user))
	{
		std::vector<const User *>::const_iterator	it = operators.begin();
		for (; it != operators.end(); ++it)
			if (*it == &user)
				break ;
		operators.erase(it);
	}
}

void				Channel::addSpeaker(const User &user)
{
	if (!isSpeaker(user))
		speakers.push_back(&user);
}

void				Channel::removeSpeaker(const User &user)
{
	if (isSpeaker(user))
	{
		std::vector<const User *>::const_iterator	it = speakers.begin();
		for (; it != speakers.end(); ++it)
			if (*it == &user)
				break ;
		speakers.erase(it);
	}
}

void				Channel::setLimit(unsigned short limit)
{
	userLimit = limit;
}

void				Channel::addBanMask(const std::string &mask)
{
	banMasks.push_back(mask);
}

void				Channel::removeBanMask(const std::string &mask)
{
	std::vector<std::string>::const_iterator	it = banMasks.begin();
	for (; it != banMasks.end(); ++it)
		if (*it == mask)
			break ;
	banMasks.erase(it);
	
}

void				Channel::disconnect(const User &user)
{
	removeOperator(user);
	removeSpeaker(user);
	std::vector<const User *>::iterator	begin = users.begin();
	std::vector<const User *>::iterator	end = users.end();
	for (; begin != end; ++begin)
		if (*begin == &user)
			break ;
	users.erase(begin);
	if (operators.size() == 0 && users.size() > 0)
		operators.push_back(users[0]);
}

void				Channel::removeInvited(const User &user)
{
	if (isInvited(user))
	{
		std::vector<const User *>::const_iterator	it = invitedUsers.begin();
		for (; it != invitedUsers.end(); ++it)
			if (*it == &user)
				break ;
		invitedUsers.erase(it);
	}
}
