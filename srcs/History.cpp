#include "History.hpp"

UserInfo::UserInfo(const User &user) :
nickname(user.getNickname()), username(user.getUsername()), hostname(user.getHostname()),
servername(user.getServername()), realname(user.getRealname()), registrationTime(time(0))
{}

const std::string	&UserInfo::getNickname() const
{
	return nickname;
}

const std::string	&UserInfo::getUsername() const
{
	return username;
}

const std::string	&UserInfo::getHostname() const
{
	return hostname;
}

const std::string	&UserInfo::getServername() const
{
	return servername;
}

const std::string	&UserInfo::getRealname() const
{
	return realname;
}

const time_t	&UserInfo::getRegistrationTime() const
{
	return registrationTime;
}

UserInfo::~UserInfo() {}

History::History() {}

History::~History()
{
	while (historyList.size() > 0)
	{
		delete historyList[0];
		historyList.erase(historyList.begin());
	}
}

void	History::addUser(const User &user)
{
	this->historyList.push_back(new UserInfo(user));
}

const std::vector<const UserInfo *>	History::getHistoryByName(const std::string &nickname) const
{
	std::vector<const UserInfo *> filteredHistory;

	for (size_t i = 0; i < historyList.size(); ++i)
	{
		if (historyList[i]->getNickname() == nickname)
			filteredHistory.push_back(historyList[i]);
	}

	return filteredHistory;
}