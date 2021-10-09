#pragma once

#include <queue>
#include <string>
#include <algorithm>

bool						newLine(char c, char sep);
bool						notNewLine(char c, char sep);
bool						isEqualToRegex(std::string mask, std::string subString);
std::queue<std::string>		split(const std::string &s, char sep, bool include);
