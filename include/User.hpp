#pragma once

class Server;
class Message;
class Channel;

#include <string>
#include <time.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <queue>
#include <set>
#include <sys/socket.h>
#include <algorithm>
#include <fcntl.h>
#include "utils.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "sendError.hpp"

#define REGISTERED		0b00000001
#define INVISIBLE		0b00000010
#define RECEIVENOTICE	0b00000100
#define RECEIVEWALLOPS	0b00001000
#define IRCOPERATOR		0b00010000
#define AWAY			0b00100000
#define PINGING			0b01000000
#define BREAKCONNECTION	0b10000000

class User
{
	private:
		int									sockfd;
		std::string							password;
		std::string							nickname;
		std::string							username;
		std::string							hostname;
		const std::string					&servername;
		std::string							realname;
		time_t								registrationTime;
		time_t								timeOfLastMessage;
		time_t								timeAfterPing;
		std::string							awayMessage;
		std::string							quitMessage;
		std::queue<std::string>				messages;
		std::vector<const Channel *>		channels;
		unsigned char						flags;

		User();
		User(const User& copy);
		User	&operator=(const User& other);
	public:
		User(int sockfd, const std::string &host, std::string &servername);
		~User();

		// Getters

		const std::string					&getUsername() const;
		const std::string					&getHostname() const;
		const std::string					&getServername() const;
		const std::string					&getNickname() const;
		const std::string					&getRealname() const;
		const std::string					&getPassword() const;
		const std::string					&getQuitMessage() const;
		const std::vector<const Channel *>	&getChannels() const;
		const std::string					&getAwayMessage() const;
		const time_t						&getRegistrationTime() const;
		const time_t						&getTimeOfLastMessage() const;
		const time_t						&getTimeAfterPing() const;
		const std::queue<std::string>		&getMessages() const;
		int									getSockfd() const;
		unsigned char						getFlags() const;
		std::string							getPrefix() const;

		// Setters

		void								setQuitMessage(const std::string &msg);
		void								setPassword(const std::string &pass);
		void								setUsername(const std::string &username);
		void								setHostname(const std::string &hostname);
		void								setNickname(const std::string &nickname);
		void								setRealname(const std::string &realname);
		void								setAwayMessage(const std::string &msg);
		void								setFlag(unsigned char flag);
		
		// Other methods

		bool								isOnChannel(const std::string &name) const;
		void								sendMessage(const std::string &msg) const;
		int									readMessage();
		void								removeChannel(const std::string &name);
		void								popMessage();
		void								addChannel(const Channel &channel);
		void								removeFlag(unsigned char flag);
		void								updateTimeOfLastMessage();
		void								updateTimeAfterPing();
};
