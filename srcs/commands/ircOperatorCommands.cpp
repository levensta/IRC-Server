#include "Server.hpp"

int Server::rehashCmd(const Message &msg, User &user) {

	(void)msg;
	if (!(user.getFlags() & IRCOPERATOR)) {
		return (sendError(user, ERR_NOPRIVILEGES));
	}

	this->loadConfig();
	return (sendReply(user.getServername(), user, RPL_REHASHING));
}

int Server::killCmd(const Message &msg, User &user) {

	if (!(user.getFlags() & IRCOPERATOR)) {
		return (sendError(user, ERR_NOPRIVILEGES));
	}

	if (msg.getParams().size() < 2) {
		return (sendError(user, ERR_NEEDMOREPARAMS));
	}

	string username = msg.getParams()[0];
	if (username == this->name) {
		return (sendError(user, ERR_CANTKILLSERVER));	
	}

	if (!containsNickname(username)) {
		sendError(user, ERR_NOSUCHNICK, msg.getParams()[0]);
	}
	User *userToKill = getUserByName(username);	
	userToKill->sendMessage(msg.getParams()[1] + "\n");
	userToKill->setFlag(BREAKCONNECTION);
	return 0;
}

int Server::restartCmd(const Message &msg, User &user) {

	(void)msg;
	if (!(user.getFlags() & IRCOPERATOR)) {
		return (sendError(user, ERR_NOPRIVILEGES));
	}

	std::vector<User *>::iterator beg = connectedUsers.begin();
	std::vector<User *>::iterator end = connectedUsers.end();
	for (; beg != end; beg++) {
		(*beg)->setFlag(BREAKCONNECTION);
	}

	loadConfig();
	close(sockfd);
	// Create a socket (IPv4, TCP)
	createSocket();

	// Listen to port on any address
	bindSocket();

	// Start listening.
	listenSocket();
	return 0;
}
