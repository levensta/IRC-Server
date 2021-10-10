#pragma once

#include <string>
#include <vector>

class User;

class UserInfo
{
	private:
		UserInfo();
		UserInfo(const UserInfo &copy);
		UserInfo &operator=(const UserInfo &copy);

		std::string							nickname;
		std::string							username;
		std::string							hostname;
		std::string							servername;
		std::string							realname;
		time_t								registrationTime;

	public:
		UserInfo(const User &user);
		~UserInfo();

		const std::string				&getNickname() const;
		const std::string				&getUsername() const;
		const std::string				&getHostname() const;
		const std::string				&getServername() const;
		const std::string				&getRealname() const;
		const time_t					&getRegistrationTime() const;
};

class History
{
	private:
		History(const History &copy);
		History &operator=(const History &copy);

		std::vector<UserInfo *>				historyList;
	
	public:
		History();
		~History();

		void								addUser(const User &user);
		const std::vector<const UserInfo *>	getHistoryByName(const std::string &nickname) const;
};

#include "User.hpp"