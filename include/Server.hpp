#pragma once

class User;
class Channel;

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <fstream>
#include <string>
#include <map>
#include "User.hpp"
#include "Channel.hpp"
#include "sendError.hpp"
#include "sendReply.hpp"

#define	DISCONNECT	-2

typedef  int (Server::*Method) (const Message &, User &);

class Server
{
	private:
		int										port;
		std::vector<User *>						connectedUsers;
		std::vector<struct pollfd>				userFDs;
		int										sockfd;
		sockaddr_in								sockaddr;
		const id_t								timeout;
		std::string								password;
		std::string								name;
		std::string								info;
		std::vector<std::string>				motd;
		std::map<std::string, Channel *>		channels;
		std::map<std::string, Method>			commands;

		Server();
		Server(const Server& copy);
		Server	&operator=(const Server& other);

		// Helpful things for commands

		int										handleChanFlags(const Message &msg, User &user);
		int										handleUserFlags(const Message &msg, User &user);
		int										checkConnection(User &user);

	public:
		Server(int port, const std::string &password);
		~Server();

		// Getters

		const int								&getSockfd() const;

		// Commands

		int										passCmd(const Message &msg, User &user);
		int										nickCmd(const Message &msg, User &user);
		int										userCmd(const Message &msg, User &user);
		int										quitCmd(const Message &msg, User &user);
		int										privmsgCmd(const Message &msg, User &user);
		int										awayCmd(const Message &msg, User &user);
		int										noticeCmd(const Message &msg, User &user);
		int										whoCmd(const Message &msg, User &user);
		int										whoisCmd(const Message &msg, User &user);
		int										modeCmd(const Message &msg, User &user);
		int										topicCmd(const Message &msg, User &user);
		int										joinCmd(const Message &msg, User &user);
		int										inviteCmd(const Message &msg, User &user);
		int										kickCmd(const Message &msg, User &user);
		int										partCmd(const Message &msg, User &user);
		int										namesCmd(const Message &msg, User &user);
		int										listCmd(const Message &msg, User &user);

		// Server setup

		void									createSocket();
		void									bindSocket();
		void									listenSocket();
		void									grabConnection();
		void									processMessages();
		int										hadleMessages(User &user);

		// Other methods

		bool									containsNickname(const std::string &nickname) const;
		bool									containsChannel(const std::string &name) const;
		void									sendMOTD(const User &user) const;
		int										connectToChannel(const User &user, const std::string &name, const std::string &key);
		void									inviteToChannel(const User &user, const std::string &nickname, const std::string &chanName);
		User									*getUserByName(const std::string &name);
};
