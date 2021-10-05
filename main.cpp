#include <string>
#include <vector>
#include <fcntl.h>
#include "User.hpp"
#include "Server.hpp"

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
		exit(EXIT_FAILURE);
	}

	int port = atoi(argv[1]);

	if (port < 1024 || port > 49151)
	{
		std::cout << "Wrong port!" << std::endl;
		exit(EXIT_FAILURE);
	}

	Server		server(port, argv[2]);

	// Create a socket (IPv4, TCP)
	server.createSocket();

	// Listen to port on any address
	server.bindSocket();

	// Start listening.
	server.listenSocket();

	fcntl(server.getSockfd(), F_SETFL, O_NONBLOCK);
	fcntl(1, F_SETFL, O_NONBLOCK);

	while (1)
	{
		char	command[20];
		command[0] = 0;
		command[read(1, command, 19)] = 0;
		if (std::string(command) == "stop\n")
			break;

		// Grab a connection from the queue
		server.grabConnection();

		server.processMessages();
	}
	close(server.getSockfd());
}