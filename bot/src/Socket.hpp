#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <netinet/tcp.h>
#include <sys/types.h>
#include <netdb.h>

#ifdef __APPLE__
#define IRC_NOSIGNAL SO_NOSIGPIPE
#else
#define IRC_NOSIGNAL MSG_NOSIGNAL
#endif

using std::string;

class Socket
{
	private:
		int _fd;
		string _host;
		in_port_t _port;
		sockaddr_in _addr;

		Socket(const Socket & other);
		Socket & operator=(const Socket & other);
		void bind(void);
		void set(void);

	public:
		Socket(const string &host, in_port_t port);
		~Socket(void);

		int getFd(void);
		void tryToConnect(void);
		int tryToSend(const string &msg);
		string tryToRecv( void );
};
