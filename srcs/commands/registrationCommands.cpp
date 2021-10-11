#include "Server.hpp"

void	Server::sendMOTD(const User &user) const
{
	if (motd.size() == 0)
		sendError(user, ERR_NOMOTD);
	else
	{
		sendReply(name, user, RPL_MOTDSTART, name);
		for (size_t i = 0; i < motd.size(); ++i)
			sendReply(name, user, RPL_MOTD, motd[i]);
		sendReply(name, user, RPL_ENDOFMOTD);
	}
}

int		Server::checkConnection(User &user)
{
	if (user.getNickname().size() > 0 && user.getUsername().size() > 0)
	{
		if (password.size() == 0 || user.getPassword() == password)
		{
			if (!(user.getFlags() & REGISTERED))
			{
				user.setFlag(REGISTERED);
				sendMOTD(user);
			}
		}
		else
			return (DISCONNECT);
	}
	return (0);
}

int		Server::passCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() == 0)
		sendError(user, ERR_NEEDMOREPARAMS, msg.getCommand());
	else if (user.getFlags() & REGISTERED)
		sendError(user, ERR_ALREADYREGISTRED);
	else
		user.setPassword(msg.getParams()[0]);
	return 0;
}

void	Server::notifyUsers(User &user, const std::string &notification)
{
	const std::vector<const Channel *> chans = user.getChannels();
	for (size_t i = 0; i < connectedUsers.size(); i++)
	{
		for (size_t j = 0; j < chans.size(); j++)
		{
			if (chans[j]->containsNickname(connectedUsers[i]->getNickname()))
			{
				connectedUsers[i]->sendMessage(notification);
				break ;
			}
		}
	}
}

int		Server::nickCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() == 0)
		sendError(user, ERR_NEEDMOREPARAMS, msg.getCommand());
	else if (!isValidNick(msg.getParams()[0]))
		sendError(user, ERR_ERRONEUSNICKNAME, msg.getParams()[0]);
	else if (this->containsNickname(msg.getParams()[0]))
		sendError(user, ERR_NICKNAMEINUSE, msg.getParams()[0]);
	else
	{
		if (user.getFlags() & REGISTERED)
		{
			notifyUsers(user, ":" + user.getPrefix() + " " + msg.getCommand() + " " + msg.getParams()[0] + "\n");
			this->nicknamesHistory.addUser(user);
		}
		user.setNickname(msg.getParams()[0]);
	}
	return (checkConnection(user));
}

int		Server::userCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() < 4)
		sendError(user, ERR_NEEDMOREPARAMS, msg.getCommand());
	else if (user.getFlags() & REGISTERED)
		sendError(user, ERR_ALREADYREGISTRED);
	else
	{ // TODO: Проверить валидность всего этого
		user.setUsername(msg.getParams()[0]);
		user.setHostname(msg.getParams()[1]);
		user.setServername(msg.getParams()[2]);
		user.setRealname(msg.getParams()[3]);
	}
	return (checkConnection(user));
}

int		Server::quitCmd(const Message &msg, User &user)
{
	if (msg.getParams().size() > 0)
		user.setQuitMessage(msg.getParams()[0]);
	this->nicknamesHistory.addUser(user);
	return (DISCONNECT);
}
