#include "utils.hpp"

bool	newLine(char c, char sep)
{
	return c == sep;
}

bool	notNewLine(char c, char sep)
{
	return !(newLine(c, sep));
}

bool	isEqualToRegex(std::string mask, std::string subString)
{
	const char *rs=0, *rp;
	const char *s = subString.c_str();
	const char *p = mask.c_str();
	while (1)
	{
		if (*p == '*')
		{
			rs = s;
			rp = ++p;
		}
		else if (!*s)
		{
			return (!(*p));
		}
		else if (*s == *p)
		{
			++s;
			++p;
		}
		else if (rs)
		{
			s = ++rs;
			p = rp;
		}
		else
		{
			return (false);
		}
	}
}

std::queue<std::string>	split(const std::string &s, char sep, bool include)
{
	std::queue<std::string>	ret;
	std::string::const_iterator	i = s.begin();
	while(i != s.end())
	{
		i = std::find_if(i, s.end(), [=](char c) { return c != sep; });
		std::string::const_iterator	j = std::find_if(i, s.end(), [=](char c) { return c == sep; });
		if (i != s.end())
		{
			if (include && j != s.end())
				ret.push(std::string(i, j + 1));
			else
				ret.push(std::string(i, j));
			i = j;
		}
	}
	return ret;
}
