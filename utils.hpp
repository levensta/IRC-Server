#ifndef UTILS_HPP
# define UTILS_HPP

# include <queue>
# include <string>
# include <algorithm>

# include "User.hpp"
# include "answers.h"

bool						newLine(char c, char sep);
bool						notNewLine(char c, char sep);
std::queue<std::string>		split(const std::string &s, char sep, bool include);
void						sendError(const User &user, int err);

#endif
