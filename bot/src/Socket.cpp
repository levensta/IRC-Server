#include "Socket.hpp"

Socket::Socket(const string &host, in_port_t port) : _host(host), _port(port) {
	
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == -1)
	{
		std::cerr << "Cannot create socket" << std::endl;
		exit(EXIT_FAILURE);
	}

	set();
}

Socket::~Socket() {
	close(_fd);
}

void Socket::set(void) {
	const int opt = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
	{
		std::cerr << "Cannot set socket options" << std::endl;
		exit(EXIT_FAILURE);
	}

	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(_port);
	_addr.sin_addr.s_addr = inet_addr(_host.c_str());

	if (_addr.sin_addr.s_addr == static_cast<in_addr_t>(-1)) {
		std::cerr << "Cannot transform socket address" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Socket::tryToConnect(void) {
		
	if (connect(_fd, (struct sockaddr *)&_addr, sizeof(_addr)) == -1) {
		std::cerr << "Cannot set socket options" << std::endl;
		exit(EXIT_FAILURE);
	}
}

int Socket::tryToSend(const string &msg) {
	if (send(_fd, msg.c_str(), msg.size(), 0) < 0) {

		return 1;
	}

	return 0;
}

string Socket::tryToRecv( void ) {
	const int _SOCK_BUFFER_SIZE = 2048;
	
	char buf[_SOCK_BUFFER_SIZE];
	if (recv(_fd, buf, _SOCK_BUFFER_SIZE, 0) < 0) {
		std::cerr << "Cannot receive data" << std::endl;
		return "";
	}
	return std::string(buf);
}

int Socket::getFd(void) {
	return _fd;
}

void Socket::bind(void) {}
Socket::Socket(const Socket &other) { *this = other; }
Socket &Socket::operator=(const Socket &other) {
	if (this != &other) {
	}
	return *this;
}
