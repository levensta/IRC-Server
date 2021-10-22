#pragma once

#include <iostream>

#include <string>

#include "JSON.hpp"
#include "Socket.hpp"
#include "Message.hpp"

using std::string;

#define BUFFER_SIZE 1024

struct message {
	string cmd;
	string msg;
	string sender;
};

class Bot
{
	private:
		string _configFile;
		bool _configLoaded;

		string _IRCatServerIP;
		string _IRCatServerPort;
		string _IRCatServerPass;
		string _username;
		string _hostname;
		string _servername;
		string _realname;
		string _nickname;
		string _API_Protocol;
		string _API_IP;
		string _API_Port;
		string _botname;

		Socket *_IRCsocket;
		Socket *_APIsocket;

		bool _isActive;
	
		void loadConfig( void );
	
	public:
		Bot(const string &filename);
		~Bot(void);
		Bot(const Bot & other);
		Bot & operator=(const Bot & other);

		bool confLoaded( void );
		bool isActive( void );

		void createSockets( void );
		void Auth( void );
		
		int sendMessage( const string &msg );
		void parseMessage( const string &msg );
		string parseAPIresponse( const string &res );
		string receiveMessage( void );
		string requestAPI( const string &name );
		void action (Message &m);

		const string getSender(const string &prefix);
		const string getLocationURL(const string &name);
};
