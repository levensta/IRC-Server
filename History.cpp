#include "History.hpp"

UserInfo::UserInfo(const User &user) :
nickname(user.getNickname()), username(user.getUsername()), hostname(user.getHostname()),
servername(user.getServername()), realname(user.getRealname()), registrationTime(user.getRegistrationTime())
{}

const std::string	&User::getNickname() const
{
	return nickname;
}

const std::string	&User::getUsername() const
{
	return username;
}

const std::string	&User::getHostname() const
{
	return hostname;
}

const std::string	&User::getServername() const
{
	return servername;
}

const std::string	&User::getRealname() const
{
	return realname;
}

const time_t	&User::getRegistrationTime() const
{
	return registrationTime;
}

UserInfo::~UserInfo() {}

History::History() {}

History::~History() {}

void	History::addUser(const User &user)
{
	this->historyList.push_back(UserInfo(user));
}

const std::vector<const UserInfo>	History::getHistoryByName(const std::string &nickname) const
{
	std::vector<const UserInfo> filteredHistory;

	for (size_t i = 0; i < historyList.size(); ++i)
	{
		if (historyList[i].getNickname() == nickname)
			filteredHistory.push_back(historyList[i]);
	}

	return filteredHistory;
}