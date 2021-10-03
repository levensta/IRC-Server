#include "Channel.hpp"

Channel::Channel(const std::string &name, const std::string &creator, const std::string &pass = "") :
name(name), mainOperator(creator), pass(pass), userLimit(10), flags(0)
{}

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

void					Channel::connect(const User &user)
{
	for (size_t i = 0; i < banMasks.size(); i++)
		if (isBanned(banMasks[i], user.getPrefix()))
			return ; 										//throw 474 ERR_BANNEDFROMCHAN
	for (size_t i = 0; i < users.size(); i++)
		if (users[i].getPrefix() == user.getPrefix())
			return ;
	users.push_back(user);
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
	for (size_t i = 0; i < users.size(); i++)
		users[i].sendMessage(msg);
}
