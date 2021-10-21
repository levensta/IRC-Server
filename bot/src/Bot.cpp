#include "Bot.hpp"
#include "Message.hpp"
#include "utils.hpp"

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

	//std::cout << IRCport << " " << APIport << std::endl;
	//std::cout << _IRCatServerIP << " " << _API_IP << std::endl;

	_IRCsocket = new Socket(_IRCatServerIP, IRCport);

	_IRCsocket->tryToConnect();
	
	fcntl(_IRCsocket->getFd(), F_SETFL, O_NONBLOCK);
	//fcntl(_APIsocket->getFd(), F_SETFL, O_NONBLOCK);

	_isActive = true;
}

void Bot::Auth(void) {

	string pass = "PASS " + _IRCatServerPass + "\n";
	string user = "USER " + _username + " " + _hostname + " " + _servername + " " + _realname  + "\n";
	string nick = "NICK " + _botname + "\n";
	
	_IRCsocket->tryToSend(pass);
	_IRCsocket->tryToSend(user);
	_IRCsocket->tryToSend(nick);

	//Check for server's response
	//string reply = _IRCsocket->tryToRecv();
	//std::cout << reply << std::endl;
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


void Bot::parseMessage(const string &msg) {

	static string last = "";

	std::queue<std::string> msgQueue = split(last + msg, '\n', true);
	while (msgQueue.size() > 0) {
		if (msgQueue.front().find('\n') != std::string::npos) {
			Message m(msgQueue.front());

			action(m);
		} 
		else {
			last = msgQueue.front();
		}
		msgQueue.pop();	
	}
}

string Bot::requestAPI( const string &name) {
	std::stringstream ss;

	ss << "GET " << getLocationURL(name) << " HTTP/1.1\r\n";

	ss << "Connection: close\r\n";
	//ss << "Host: 165.22.89.163:5000 \r\n";
 	//ss << "Connection: keep-alive \r\n";
 	//ss << "Cache-Control: max-age=0 \r\n";
	//ss << "Upgrade-Insecure-Requests: 1\r\n";
 	//ss << "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/94.0.4606.61 Safari/537.36\r\n";
 	//ss << "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n";
 	//ss << "Accept-Encoding: gzip, deflate\r\n";
 	//ss << "Accept-Language': 'en-US,en;q=0.9,ru-RU;q=0.8,ru;q=0.7,ja-JP;q=0.6,ja;q=0.5\r\n";
	ss << "\n\n";
    
	std::cout << ss.str() << std::endl;
	_APIsocket->tryToSend(ss.str());
	
	char buf[10024] = {0};
	sleep(5); //Remove maybe
	int res = read(_APIsocket->getFd(), buf, 10023);
	std::cout << "res = "<< res << std::endl;
	std::cout << "buf = "<< buf << std::endl;
	return string(buf);
}

string Bot::parseAPIresponse( const string &res ) {
	int pos = res.find("\r\n\r\n");
	if (res == "")
		return "";
	return res.substr(pos + 4);
}

void Bot::action( Message &m ) {

	std::cout << m.getCommand() << std::endl;
	if (m.getCommand() == "PRIVMSG") {

		int APIport = static_cast<in_port_t>(strtol(_API_Port.c_str(), NULL, 10));
		_APIsocket = new Socket(_API_IP, APIport);
		_APIsocket->tryToConnect();
		
		string res = requestAPI(m.getParams()[1]);
		//std::cout << res << std::endl;
		string body = parseAPIresponse(res);

		delete _APIsocket;

		char sender[512] = {0};
		sscanf(m.getPrefix().c_str(), "%[^!]", sender);
		string ssender(sender);
		sendMessage("PRIVMSG " + ssender + " :" + body + "\n");
	
	} else if (m.getCommand() == "PING") {

		sendMessage("PONG :" + m.getParams()[0]);
	
	} else {
		//ignore other
	}
}

bool Bot::isActive( void ) {
	return _isActive;
}

int Bot::sendMessage(const string &msg) {
	return _IRCsocket->tryToSend(msg);
}

const string Bot::getLocationURL(const string &name) {
	return "/users/" + name + "/location";
}
