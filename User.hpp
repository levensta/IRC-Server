#ifndef USER_HPP
# define USER_HPP

class Server;
class Message;
class Channel;

# include <string>
# include <iostream>
# include <sstream>
# include <unistd.h>
# include <vector>
# include <queue>
# include <set>
# include <sys/socket.h>
# include <algorithm>
# include <fcntl.h>
# include "utils.hpp"
# include "Message.hpp"
# include "Server.hpp"
# include "Channel.hpp"
# include "sendError.hpp"

enum Role
{
	client,
	IrcOperator,
};

class User
{
	private:
		Server							*server;
		bool							enterUsername;
		bool							enterNickname;
		bool							registered;
		bool							away;
		bool							invisible;
		bool							receiveNotice;
		bool							receiveWallops;
		Role							role;
		std::string						password;
		std::string						nickname;
		std::string						username;
		std::string						hostname;
		std::string						servername;
		std::string						realname;
		std::string						awayMessage;
		int								sockfd;
		std::queue<std::string>			messages;
		std::vector<Channel *>			channels;

		Channel							*getChanByName(const std::string &name);

		bool							isValidNick(const std::string &nick) const;
		bool							isValidChannelName(const std::string &name) const;
		bool							isOnChannel(const std::string &name) const;

		void							passCmd(const Message &msg);
		int								nickCmd(const Message &msg);
		int								userCmd(const Message &msg);
		void							joinCmd(const Message &msg);
		void 							privmsgCmd(const Message &msg);
		void							awayCmd(const Message &msg);
		void							noticeCmd(const Message &msg);
		void							inviteCmd(const Message &msg);
		void							modeCmd(const Message &msg);
		void							topicCmd(const Message &msg);
		void							namesCmd(const Message &msg);
		void							kickCmd(const Message &msg);
		int								partCmd(const Message &msg);

		int								checkConnection();
		int								handleChanFlags(const Message &msg);
		int								handleUserFlags(const Message &msg);

		User();
		User(const User& copy);
		User	&operator=(const User& other);
	public:
		User(int sockfd, Server &server);
		~User();

		int								getSockfd() const;
		const std::string				&getNickname() const;
		const std::string				&getServername() const;
		std::string						getPrefix() const;
		const std::string				&getAwayMessage() const;
		bool							isAway() const;
		bool							isInvisible() const;
		bool							isReceiveNotice() const;
		bool							isReceiveWallops() const;

		void							readMessage();
		std::vector<std::string>		parseCommand();
		int								hadleMessages();
		void							sendMessage(const std::string &msg) const;
		void							removeChannel(const std::string &name);
};

#endif
