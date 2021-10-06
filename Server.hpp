#ifndef SERVER_HPP
# define SERVER_HPP

class User;
class Channel;

# include <sys/socket.h> // For socket functions
# include <netinet/in.h> // For sockaddr_in
# include <cstdlib> // For exit() and EXIT_FAILURE
# include <iostream> // For cout
# include <unistd.h>
# include <errno.h>
# include <poll.h>
# include <fstream>
# include <string>
# include "User.hpp"
# include "Channel.hpp"
# include "sendError.hpp"
# include "sendReply.hpp"

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
		std::vector<std::string>				motd;
		std::map<std::string, Channel *>		channels;

		Server();
		Server(const Server& copy);
		Server	&operator=(const Server& other);
	public:
		Server(int port, const std::string &password);
		~Server();

		const int								&getSockfd() const;
		const std::string						&getPassword() const;
		const std::string						&getServername() const;
		const std::map<std::string, Channel *>	&getChannels() const;
		bool									containsNickname(const std::string &nickname) const;

		void									createSocket();
		void									bindSocket();
		void									listenSocket();
		void									grabConnection();
		void									processMessages();
		void									sendMOTD(const User &user) const;
		int										connectToChannel(const User &user, const std::string &name, const std::string &key);
};

#endif
