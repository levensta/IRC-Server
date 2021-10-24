#pragma once

class Message;

#include <queue>
#include <string>
#include <algorithm>
#include <iostream>
#include "Message.hpp"

bool						newLine(char c, char sep);
bool						notNewLine(char c, char sep);
bool						isEqualToRegex(std::string mask, std::string subString);
std::queue<std::string>		split(const std::string &s, char sep, bool include);
void						logMessage(const Message &msg);
bool						isValidNick(const std::string &nick);
bool						isValidChannelName(const std::string &name);
