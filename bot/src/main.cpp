#include "Bot.hpp"

int main(int argc, char *argv[]) {

	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <bot-config.json>" << std::endl;
		return 1;
	}

	string filename(argv[1]);

	Bot bot(filename);
	
	if (!bot.confLoaded()) {
		std::cerr << "Cannot load bot configuration from " << filename << std::endl;
		return 1;
	}
	
	bot.Auth();
	
	string APIres;
	string IRCmsg;
	string name;
	while (bot.isActive()) {
		IRCmsg = bot.receiveMessage();
		name = bot.parseMessage(IRCmsg);
		APIres = bot.requestAPI(name);
		bot.sendMessage(APIres);
	}

	return 0;
}
