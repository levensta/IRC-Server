#ifndef SERVER_HPP
# define SERVER_HPP

# include "User.hpp"
# include <sys/socket.h> // For socket functions
# include <netinet/in.h> // For sockaddr_in
# include <cstdlib> // For exit() and EXIT_FAILURE
# include <iostream> // For cout
# include <unistd.h>
# include <errno.h>

class Server
{
	private:
		int					port;
		std::vector<User>	connections;
		int					sockfd;
		sockaddr_in			sockaddr;
	public:
		Server(int port);
		~Server();
		int					getSockfd() const;
		void				createSocket();
		void				bindSocket();
		void				listenSocket();
		void				grabConnection();
		void				processMessages();
};

#endif
