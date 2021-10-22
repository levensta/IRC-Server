#include "sendReply.hpp"

int		sendReply(const std::string &from, const User &user, int rpl, \
				const std::string &arg1,const std::string &arg2, \
				const std::string &arg3,const std::string &arg4, \
				const std::string &arg5,const std::string &arg6, \
				const std::string &arg7,const std::string &arg8)
{
	std::string	msg = ":" + from + " ";
	std::stringstream	ss;
	ss << rpl;
	msg += ss.str() + " " + user.getNickname() + " ";
	switch (rpl)
	{
	case RPL_USERHOST:
		msg += ":" + arg1 + "\n";
		break;
	case RPL_ISON:
		msg += ":" + arg1 + "\n";
		break;
	case RPL_AWAY:
		msg += arg1 + " :" + arg2 + "\n";
		break;
	case RPL_UNAWAY:
		msg += ":You are no longer marked as being away\n";
		break;
	case RPL_NOWAWAY:
		msg += ":You have been marked as being away\n";
		break;
	case RPL_WHOISUSER:
		msg += arg1 + " " + arg2 + " " + arg3 + " * :" + arg4 + "\n";
		break;
	case RPL_WHOISSERVER:
		msg += arg1 + " " + arg2 + " :" + arg3 + "\n";
		break;
	case RPL_WHOISOPERATOR:
		msg += arg1 + " :is an IRC operator\n";
		break;
	case RPL_WHOISIDLE:
		msg += arg1 + " " + arg2 + " " + arg3 + " :seconds idle\n";
		break;
	case RPL_ENDOFWHOIS:
		msg += arg1 + " :End of /WHOIS list\n";
		break;
	case RPL_WHOISCHANNELS:
		msg += arg1 + " :" + arg2 + "\n";
		break;
	case RPL_WHOWASUSER:
		msg += arg1 + " " + arg2 + " " + arg3 + " * :" + arg4 + "\n";
		break;
	case RPL_ENDOFWHOWAS:
		msg += arg1 + " :End of WHOWAS\n";
		break;
	case RPL_LISTSTART:
		msg += "Channel :Users  Name\n";
		break;
	case RPL_LIST:
		msg += arg1 +  " " + arg2 + " :" + arg3 + "\n";
		break;
	case RPL_LISTEND:
		msg += ":End of /LIST\n";
		break;
	case RPL_CHANNELMODEIS:
		msg += arg1 + " +" + arg2 + "\n";
		break;
	case RPL_NOTOPIC:
		msg += arg1 + " :No topic is set\n";
		break;
	case RPL_TOPIC:
		msg += arg1 + " :" + arg2 + "\n";
		break;
	case RPL_INVITING:
		msg += arg1 + " " + arg2 + "\n";
		break;
	case RPL_SUMMONING:
		msg += arg1 + " :Summoning user to IRC\n";
		break;
	case RPL_VERSION:
		msg += arg1 + "." + arg2 + " " + arg3 + " :" + arg4 + "\n";
		break;
	case RPL_WHOREPLY:
		msg += arg1 + " " + arg2 + " " + arg3 + " " + arg4 + " ";
		msg += arg5 + " " + arg6 + " :" + arg7 + " " + arg8 + "\n";
		break;
	case RPL_ENDOFWHO:
		msg += arg1 + " :End of /WHO list\n";
		break;
	case RPL_NAMREPLY:
		msg += arg1 + " :" + arg2 + "\n";
		break;
	case RPL_ENDOFNAMES:
		msg += arg1 + " :End of /NAMES list\n";
		break;
	case RPL_LINKS:
		msg += arg1 + " " + arg2 + ": " + arg3 + " " + arg4 + "\n";
		break;
	case RPL_ENDOFLINKS:
		msg += arg1 + " :End of /LINKS list\n";
		break;
	case RPL_BANLIST:
		msg += arg1 + " " + arg2 + "\n";
		break;
	case RPL_ENDOFBANLIST:
		msg += arg1 + " :End of channel ban list\n";
		break;
	case RPL_INFO:
		msg += ":" + arg1 + "\n";
		break;
	case RPL_ENDOFINFO:
		msg += ":End of /INFO list\n";
		break;
	case RPL_MOTDSTART:
		msg += ":- " + arg1 + " Message of the day - \n";
		break;
	case RPL_MOTD:
		msg += ":- " + arg1 + "\n";
		break;
	case RPL_ENDOFMOTD:
		msg += ":End of /MOTD command\n";
		break;
	case RPL_YOUREOPER:
		msg += ":You are now an IRC operator\n";
		break;
	case RPL_REHASHING:
		msg += arg1 + " :Rehashing\n";
		break;
	case RPL_TIME:
		msg += arg1 + " :" + arg2; // ctime return string following '\n'
		break;
	case RPL_USERSSTART:
		msg += ":UserID   Terminal  Host\n";
		break;
	case RPL_USERS:
		msg += ":%-8s %-9s %-8s\n"; // Хз, что это UPD: Понял, строка длиной 8 символов
		break;
	case RPL_ENDOFUSERS:
		msg += ":End of users\n";
		break;
	case RPL_NOUSERS:
		msg += ":Nobody logged in\n";
		break;
	case RPL_TRACELINK:
		msg += "Link " + arg1 + " " + arg2 + " " + arg3 + "\n";
		break;
	case RPL_TRACECONNECTING:
		msg += "Try. " + arg1 + " " + arg2 + "\n";
		break;
	case RPL_TRACEHANDSHAKE:
		msg += "H.S. " + arg1 + " " + arg2 + "\n";
		break;
	case RPL_TRACEUNKNOWN:
		msg += "???? " + arg1 + " " + arg2 + "\n"; // ????
		break;
	case RPL_TRACEOPERATOR:
		msg += "Oper " + arg1 + " " + arg2 + "\n";
		break;
	case RPL_TRACEUSER:
		msg += "User " + arg1 + " " + arg2 + "\n";
		break;
	case RPL_TRACESERVER:
		msg += "Serv " + arg1 + " " + arg2 + "S " + arg3 + "C ";
		msg += arg4 + " " + arg5 + "@" + arg6 + "\n";
		break;
	case RPL_TRACENEWTYPE:
		msg += arg1 + " 0 " + arg2 + "\n";
		break;
	case RPL_TRACELOG:
		msg += "File " + arg1 + " " + arg2 + "\n";
		break;
	case RPL_STATSLINKINFO:
		msg += arg1 + " " + arg2 + " " + arg3 + " " + arg4 + " ";
		msg += arg5 + " " + arg6 + " " + arg7 + "\n";
		break;
	case RPL_STATSCOMMANDS:
		msg += arg1 + " " + arg2 + "\n";
		break;
	case RPL_STATSCLINE:
		msg += "C " + arg1 + " * " + arg2 + " " + arg3 + " " + arg4 + "\n";
		break;
	case RPL_STATSNLINE:
		msg += "N " + arg1 + " * " + arg2 + " " + arg3 + " " + arg4 + "\n";
		break;
	case RPL_STATSILINE:
		msg += "I " + arg1 + " * " + arg2 + " " + arg3 + " " + arg4 + "\n";
		break;
	case RPL_STATSKLINE:
		msg += "K " + arg1 + " * " + arg2 + " " + arg3 + " " + arg4 + "\n";
		break;
	case RPL_STATSYLINE:
		msg += "Y " + arg1 + " * " + arg2 + " " + arg3 + " " + arg4 + "\n";
		break;
	case RPL_ENDOFSTATS:
		msg += arg1 + " :End of /STATS report\n";
		break;
	case RPL_STATSLLINE:
		msg += "L " + arg1 + " * " + arg2 + " " + arg3 + "\n";
		break;
	case RPL_STATSUPTIME:
		msg += ":Server Up %d days %d:%02d:%02d\n"; // Поменять
		break;
	case RPL_STATSOLINE:
		msg += "O " + arg1 + " * " + arg2 + "\n";
		break;
	case RPL_STATSHLINE:
		msg += "H " + arg1 + " * " + arg2 + "\n";
		break;
	case RPL_UMODEIS:
		msg += arg1 + "\n";
		break;
	case RPL_LUSERCLIENT:
		msg += ":There are " + arg1 + " users and " + arg2;
		msg += " invisible on " + arg3 + " servers\n";
		break;
	case RPL_LUSEROP:
		msg += arg1 + " :operator(s) online\n";
		break;
	case RPL_LUSERUNKNOWN:
		msg += arg1 + " :unknown connection(s)\n";
		break;
	case RPL_LUSERCHANNELS:
		msg += arg1 + " :channels formed\n";
		break;
	case RPL_LUSERME:
		msg += ":I have " + arg1 + " clients and " + arg2 + " servers\n";
		break;
	case RPL_ADMINME:
		msg += arg1 + " :Administrative info\n";
		break;
	case RPL_ADMINLOC1:
		msg += ":Name     " + arg1 + "\n";
		break;
	case RPL_ADMINLOC2:
		msg += ":Nickname " + arg1 + "\n";
		break;
	case RPL_ADMINEMAIL:
		msg += ":E-Mail   " + arg1 + "\n";
		break;
	default:
		msg += "UNKNOWN REPLY\n";
		break;
	}
	send(user.getSockfd(), msg.c_str(), msg.size(), IRC_NOSIGNAL);
	return 0;
}