#include "Server.hpp"

int		Server::wallopsCmd(const Message &msg, User &user)
{
	if (!(user.getFlags() & IRCOPERATOR))
		return (sendError(user, ERR_NOPRIVILEGES));
	if (msg.getParams().size() == 0)
		return (sendError(user, ERR_NEEDMOREPARAMS, msg.getCommand()));

	const std::vector<User *> usersList = this->connectedUsers;
	for (size_t i = 0; i < usersList.size(); ++i)
	{
		if (usersList[i]->getFlags() & IRCOPERATOR)
			usersList[i]->sendMessage(":" + user.getPrefix() + " " \
			+ msg.getCommand() + " :" + msg.getParams()[0] + "\n");
	}
	return 0;
}

int		Server::pingCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() == 0)
		return (sendError(user, ERR_NOORIGIN));
	user.sendMessage(":" + this->name + " PONG :" + msg.getParams()[0] + "\n");
	return 0;
}

int		Server::pongCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() <= 0 || msg.getParams()[0] != this->name)
		return (sendError(user, ERR_NOSUCHSERVER, msg.getParams().size() > 0 ? msg.getParams()[0] : ""));
	user.removeFlag(PINGING);
	return 0;
}

int		Server::isonCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() == 0)
		return (sendError(user, ERR_NEEDMOREPARAMS, msg.getCommand()));

	std::string	nicknamesList;
	for (size_t i = 0; i < msg.getParams().size(); ++i)
	{
		if (this->containsNickname(msg.getParams()[i]))
		{
			if (nicknamesList.size() > 0)
				nicknamesList += " ";
			nicknamesList += msg.getParams()[i];
		}
	}
	return (sendReply(user.getServername(), user, RPL_ISON, nicknamesList));
}

int		Server::userhostCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() == 0)
		return (sendError(user, ERR_NEEDMOREPARAMS, msg.getCommand()));

	std::string	replyMessage;
	for (size_t i = 0; i < msg.getParams().size() && i < 5; ++i)
	{
		if (this->containsNickname(msg.getParams()[i]))
		{
			User *currentUser = this->getUserByName(msg.getParams()[i]);
			if (replyMessage.size() > 0)
				replyMessage += " ";
			replyMessage += msg.getParams()[i];
			if (currentUser->getFlags() & IRCOPERATOR)
				replyMessage += "*";
			replyMessage += (currentUser->getFlags() & AWAY) ? "=-@" : "=+@";
			replyMessage += currentUser->getHostname();
		}
	}
	return (sendReply(user.getServername(), user, RPL_USERHOST, replyMessage));
}

int		Server::versionCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() > 0 && msg.getParams()[0] != user.getServername())
		return (sendError(user, ERR_NOSUCHSERVER, msg.getParams()[0]));
	return (sendReply(user.getServername(), user, RPL_VERSION, version, debuglvl, name, comments));
}

int		Server::infoCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() > 0 && msg.getParams()[0] != user.getServername())
		return (sendError(user, ERR_NOSUCHSERVER, msg.getParams()[0]));
	std::queue<std::string>	lines = split(describe, '\n', false);
	for (;lines.size() > 0; lines.pop())
		sendReply(user.getServername(), user, RPL_INFO, lines.front());
	sendReply(user.getServername(), user, RPL_INFO, info);
	return (sendReply(user.getServername(), user, RPL_ENDOFINFO));
}

int		Server::adminCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() > 0 && msg.getParams()[0] != user.getServername())
		return (sendError(user, ERR_NOSUCHSERVER, msg.getParams()[0]));
	sendReply(user.getServername(), user, RPL_ADMINME, user.getServername());
	sendReply(user.getServername(), user, RPL_ADMINLOC1, adminName);
	sendReply(user.getServername(), user, RPL_ADMINLOC2, adminNickname);
	sendReply(user.getServername(), user, RPL_ADMINEMAIL, adminEmail);
	return 0;
}

int		Server::timeCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() > 0 && msg.getParams()[0] != user.getServername())
		return (sendError(user, ERR_NOSUCHSERVER, msg.getParams()[0]));
	time_t tmp = time(0);
	return (sendReply(user.getServername(), user, RPL_TIME, user.getServername(), ctime(&tmp)));
}
