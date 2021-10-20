#include "Bot.hpp"

Bot::Bot(const string &filename) 
: _configFile(filename), _configLoaded(false), _IRCsocket(0), _APIsocket(0), _isActive(false) {
	loadConfig();
}

Bot::~Bot() {
	delete _IRCsocket;
	delete _APIsocket;
}

Bot::Bot(const Bot &other) {
	*this = other;
}

Bot &Bot::operator=(const Bot &other) {
	if (this != &other) {
		_configFile = other._configFile;
		_configLoaded = other._configLoaded;

		_IRCatServerIP = other._IRCatServerIP;
		_IRCatServerPort = other._IRCatServerPort;
		_IRCatServerPass = other._IRCatServerPass;
		_username = other._username;
		_hostname = other._hostname;
		_servername = other._servername;
		_realname = other._realname;
		_nickname = other._nickname;
		_API_Protocol = other._API_Protocol;
		_API_IP = other._API_IP;
		_API_Port = other._API_Port;
	}
	return *this;
}

void Bot::loadConfig( void ) {
	JSON::JSON json(_configFile);

	json.loadFile();

	JSON::Object *conf = json.parse();

	if (conf == NULL) {
		_configLoaded = false;
	}
	else {
		_IRCatServerIP = conf->get("IRCatServerIP")->toStr();
		_IRCatServerPort = conf->get("IRCatServerPort")->toStr();
		_IRCatServerPass = conf->get("IRCatServerPass")->toStr();
		_username = conf->get("Username")->toStr();
		_hostname = conf->get("Hostname")->toStr();
		_servername = conf->get("Servername")->toStr();
		_realname = conf->get("Realname")->toStr();
		_nickname = conf->get("Nickname")->toStr();
		_API_Protocol = conf->get("API-protocol")->toStr();
		_API_IP = conf->get("API-ip")->toStr();
		_API_Port = conf->get("API-port")->toStr();
		_botname = conf->get("Botname")->toStr();

		delete conf;

		_configLoaded = true;
	}
}

bool Bot::confLoaded( void ) {
	return _configLoaded;
}

void Bot::createSockets( void ) {
	int IRCport = static_cast<in_port_t>(strtol(_IRCatServerPort.c_str(), NULL, 10));
	int APIport = static_cast<in_port_t>(strtol(_API_Port.c_str(), NULL, 10));

	_IRCsocket = new Socket(_IRCatServerIP, IRCport);
	_APIsocket = new Socket(_API_IP, APIport);

	_IRCsocket->tryToConnect();
	_APIsocket->tryToConnect();

	_isActive = true;
}

const string Bot::getAuthMessage() {
	std::stringstream ss;

	ss << "PASS " << _IRCatServerPass << " \r\n";
	ss << "USER " << _username << " " << _hostname << " ";
	ss << _servername << " " << _realname << " \r\n";
	ss << "NICK " << _botname << " \r\n";

	return ss.str();
}

void Bot::Auth(void) {
	string msg = getAuthMessage();

	_IRCsocket->tryToSend(msg);

	//Check for server's response
	string reply = _IRCsocket->tryToRecv();
}

int findnth(const std::string &haystack, const std::string &needle, int nth)
{
    size_t  pos = 0;
    int     cnt = 0;

    while( cnt != nth )
    {
        pos++;
        pos = haystack.find(needle, pos);
        if ( pos == std::string::npos )
            return -1;
        cnt++;
    }
    return pos;
}

string Bot::receiveMessage() {
	string req = _IRCsocket->tryToRecv();

	if (req == "") {
		_isActive = false;
	} 
	return req; 
}

int Bot::parseMessage(const string &msg) {
	int res = findnth(msg, ":", 2);
	if (res != -1) {
		int pos = msg.find_first_of(" ", res);

		msg.substr(res, pos - res);
	}
	//PING
	//PRIVMSG
	return 0;
}

string Bot::requestAPI( const string &name) {
	std::stringstream ss;

	ss << "GET " << getLocationURL(name) << " HTTP/1.1 \r\n";
	
	_APIsocket->tryToSend(ss.str());
	return _APIsocket->tryToRecv();
}

bool Bot::isActive( void ) {
	return _isActive;
}

int Bot::sendMessage(const string &msg) {
	return _IRCsocket->tryToSend(msg);
}

const string Bot::getLocationURL(const string &name) {
	return "/users" + name + "/location";
}
