#include "Channel.hpp"

Channel::Channel(const std::string &name, const User &creator, const std::string &pass) :
name(name), pass(pass), userLimit(0), flags(0)
{
	users[&creator] = time(0);
	operators.push_back(&creator);
}

Channel::~Channel()
{}

const std::string		&Channel::getName() const
{
	return (name);
}

void					Channel::setTopic(const std::string &topic)
{
	this->topic = topic;
}

bool					isBanned(const std::string &mask, const std::string &prefix)
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

bool						Channel::isInvited(const User &user)
{
	for (size_t i = 0; i < invitedUsers.size(); i++)
		if (invitedUsers[i]->getPrefix() == user.getPrefix())
			return true;
	return false;
}

void					Channel::connect(const User &user, const std::string &key)
{
	if (flags & PRIVATE && key != pass)
		sendError(user, ERR_BADCHANNELKEY, name);
	else if (userLimit != 0 && users.size() >= userLimit)
		sendError(user, ERR_CHANNELISFULL, name);
	else if (flags & INVITEONLY && !isInvited(user))
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
		std::map<const User *, time_t>::iterator	begin = users.begin();
		std::map<const User *, time_t>::iterator	end = users.end();
		for (; begin != end; ++begin)
			if ((*begin).first->getPrefix() == user.getPrefix())
				return ;
		users[&user] = time(0);
		// call TOPIC
	}
}

void					Channel::setFlag(unsigned char flag)
{
	flags |= flag;
}

void					Channel::removeFlag(unsigned char flag)
{
	flags &= ~flag;
}

void					Channel::sendMessage(const std::string &message, const User &from)
{
	std::string	msg;
	msg += ":" + from.getPrefix() + " " + message;
		std::map<const User *, time_t>::iterator	begin = users.begin();
		std::map<const User *, time_t>::iterator	end = users.end();
	for (; begin != end; ++begin)
		(*begin).first->sendMessage(msg);
}
