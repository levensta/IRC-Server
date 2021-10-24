#include "sendError.hpp"

int		sendError(const User &user, int err, const std::string &arg1, const std::string &arg2)
{
	std::string	msg = ":" + user.getServername() + " ";
	std::stringstream	ss;
	ss << err;
	msg += ss.str() + " " + user.getNickname();
	switch (err)
	{
	case ERR_NOSUCHNICK:
		msg += " " + arg1 + " :No such nick/channel\n";
		break;
	case ERR_NOSUCHSERVER:
		msg += " " + arg1 + " :No such server\n";
		break;
	case ERR_NOSUCHCHANNEL:
		msg += " " + arg1 + " :No such channel\n";
		break;
	case ERR_CANNOTSENDTOCHAN:
		msg += " " + arg1 + " :Cannot send to channel\n";
		break;
	case ERR_TOOMANYCHANNELS:
		msg += " " + arg1 + " :You have joined too many channels\n";
		break;
	case ERR_WASNOSUCHNICK:
		msg += " " + arg1 + " :There was no such nickname\n";
		break;
	case ERR_TOOMANYTARGETS:
		msg += " " + arg1 + " :Duplicate recipients. No arg1 delivered\n";
		break;
	case ERR_NOORIGIN:
		msg += " :No origin specified\n";
		break;
	case ERR_NORECIPIENT:
		msg += " :No recipient given (" + arg1 + ")\n";
		break;
	case ERR_NOTEXTTOSEND:
		msg += " :No text to send\n";
		break;
	case ERR_NOTOPLEVEL:
		msg += " " + arg1 + " :No toplevel domain specified\n";
		break;
	case ERR_WILDTOPLEVEL:
		msg += " " + arg1 + " :Wildcard in toplevel domain\n";
		break;
	case ERR_UNKNOWNCOMMAND:
		msg += " " + arg1 + " :Unknown command\n";
		break;
	case ERR_NOMOTD:
		msg += " :MOTD File is missing\n";
		break;
	case ERR_NOADMININFO:
		msg += " " + arg1 + " :No administrative info available\n";
		break;
	case ERR_FILEERROR:
		msg += " :File error doing \n" + arg1 + " on " + arg2 + "\n";
		break;
	case ERR_NONICKNAMEGIVEN:
		msg += " :No nickname given\n";
		break;
	case ERR_ERRONEUSNICKNAME:
		msg += " " + arg1 + " :Erroneus nickname\n";
		break;
	case ERR_NICKNAMEINUSE:
		msg += " " + arg1 + " :Nickname is already in use\n";
		break;
	case ERR_NICKCOLLISION:
		msg += " " + arg1 + " :Nickname collision KILL\n";
		break;
	case ERR_USERNOTINCHANNEL:
		msg += " " + arg1 + " " + arg2 + " :They aren't on that channel\n";
		break;
	case ERR_NOTONCHANNEL:
		msg += " " + arg1 + " :You're not on that channel\n";
		break;
	case ERR_USERONCHANNEL:
		msg += " " + arg1 + " " + arg2 + " :is already on channel\n";
		break;
	case ERR_NOLOGIN:
		msg += " " + arg1 + " :User not logged in\n";
		break;
	case ERR_SUMMONDISABLED:
		msg += " :SUMMON has been disabled\n";
		break;
	case ERR_USERSDISABLED:
		msg += " :USERS has been disabled\n";
		break;
	case ERR_NOTREGISTERED:
		msg += " :You have not registered\n";
		break;
	case ERR_NEEDMOREPARAMS:
		msg += " " + arg1 + " :Not enough parameters\n";
		break;
	case ERR_ALREADYREGISTRED:
		msg += " :You may not reregister\n";
		break;
	case ERR_NOPERMFORHOST:
		msg += " :Your host isn't among the privileged\n";
		break;
	case ERR_PASSWDMISMATCH:
		msg += " :Password incorrect\n";
		break;
	case ERR_YOUREBANNEDCREEP:
		msg += " :You are banned from this server\n";
		break;
	case ERR_KEYSET:
		msg += " " + arg1 + " :Channel key already set\n";
		break;
	case ERR_CHANNELISFULL:
		msg += " " + arg1 + " :Cannot join channel (+l)\n";
		break;
	case ERR_UNKNOWNMODE:
		msg += " " + arg1 + " :is unknown mode char to me\n";
		break;
	case ERR_INVITEONLYCHAN:
		msg += " " + arg1 + " :Cannot join channel (+i)\n";
		break;
	case ERR_BANNEDFROMCHAN:
		msg += " " + arg1 + " :Cannot join channel (+b)\n";
		break;
	case ERR_BADCHANNELKEY:
		msg += " " + arg1 + " :Cannot join channel (+k)\n";
		break;
	case ERR_NOPRIVILEGES:
		msg += " :Permission Denied- You're not an IRC operator\n";
		break;
	case ERR_CHANOPRIVSNEEDED:
		msg += " " + arg1 + " :You're not channel operator\n";
		break;
	case ERR_CANTKILLSERVER:
		msg += " :You cant kill a server!\n";
		break;
	case ERR_NOOPERHOST:
		msg += " :No O-lines for your host\n";
		break;
	case ERR_UMODEUNKNOWNFLAG:
		msg += " :Unknown MODE flag\n";
		break;
	case ERR_USERSDONTMATCH:
		msg += " :Cant change mode for other users\n";
		break;
	default:
		msg += "UNKNOWN ERROR\n";
		break;
	}
	send(user.getSockfd(), msg.c_str(), msg.size(), IRC_NOSIGNAL);
	return (-1);
}
