#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For read
#include <string>
#include <vector>
#include <fcntl.h>
#include "User.hpp"

void	grabConnection(int sockfd, sockaddr_in sockaddr, std::vector<User> &connections)
{
	size_t addrlen = sizeof(sockaddr);
	int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
	if (connection >= 0)
		connections.push_back(User(connection));
}

void	processMessages(std::vector<User> &connections)
{
	std::vector<User>::iterator	begin = connections.begin();
	std::vector<User>::iterator	end = connections.end();
	// Read from the connection
	for (; begin != end; begin++)
	{
		(*begin).readMessage();
		(*begin).hadleMessages();
	}
}

int main(int argc, char **argv)
{
	int port = atoi(argv[1]);
	std::vector<User>	connections;

	// Create a socket (IPv4, TCP)
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	// Listen to port on any address
	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(port); // htons is necessary to convert a number to network byte order
	if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
	{
		std::cout << "Failed to bind to port " << port << ". errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	// Start listening. Hold at most 10 connections in the queue
	if (listen(sockfd, 10) < 0)
	{
		std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	fcntl(sockfd, F_SETFL, O_NONBLOCK);

	while (1)
	{
		// Grab a connection from the queue
		grabConnection(sockfd, sockaddr, connections);

		processMessages(connections);
	}
	close(sockfd);
}