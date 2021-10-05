#ifndef SERVER_HPP
# define SERVER_HPP

class User;

# include "User.hpp"
# include <sys/socket.h> // For socket functions
# include <netinet/in.h> // For sockaddr_in
# include <cstdlib> // For exit() and EXIT_FAILURE
# include <iostream> // For cout
# include <unistd.h>
# include <errno.h>
# include <poll.h>
# include "sendError.hpp"

class Server
{
	private:
		int							port;
		std::vector<User>			connectedUsers;
		std::vector<struct pollfd>	userFDs;
		int							sockfd;
		sockaddr_in					sockaddr;
		const id_t					timeout;
		std::string					password;
		std::string					name;
		std::string					motd;
	public:
		Server(int port, const std::string &password);
		~Server();

		const int					&getSockfd() const;
		const std::string			&getPassword() const;
		const std::string			&getServername() const;
		bool						containsNickname(const std::string &nickname) const;

		void						createSocket();
		void						bindSocket();
		void						listenSocket();
		void						grabConnection();
		void						processMessages();
		void						sendMOTD(const User &user) const;
};

#endif
