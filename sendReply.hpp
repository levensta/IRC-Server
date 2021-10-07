#ifndef SENDREPLY_HPP
# define SENDREPLY_HPP


# include "User.hpp"
# include "answers.h"

void	sendReply(const std::string &from, const User &user, int rpl, \
				const std::string &arg1 = "",const std::string &arg2 = "", \
				const std::string &arg3 = "",const std::string &arg4 = "", \
				const std::string &arg5 = "",const std::string &arg6 = "", \
				const std::string &arg7 = "",const std::string &arg8 = "");


#endif
