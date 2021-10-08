#ifndef SENDERROR_HPP
# define SENDERROR_HPP

# include "User.hpp"
# include "answers.h"

int		sendError(const User &user, int err, const std::string &arg1 = "", const std::string &arg2 = "");

#endif
