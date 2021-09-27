#include "Message.hpp"

Message::Message(std::string &str)
{
	std::string	strWithoutNL = std::string(str.begin(), str.end() - 1);
	std::queue<std::string>	que = split(strWithoutNL, ' ', false);
	if (que.size() > 0 && que.front()[0] == ':')
	{
		prefix = std::string(que.front().begin() + 1, que.front().end());
		que.pop();
	}
	if (que.size() > 0)
	{
		command = que.front();
		que.pop();
	}
	while (que.size() > 0)
	{
		if (que.front()[0] == ':')
		{
			std::string	s(que.front().begin() + 1, que.front().end());
			que.pop();
			while (que.size() > 0)
			{
				s.append(" ");
				s.append(que.front());
				que.pop();
			}
			parametrs.push_back(s);
		}
		else
		{
			parametrs.push_back(que.front());
			que.pop();
		}
	}
}

std::string					Message::getPrefix() const
{
	return prefix;
}

std::string					Message::getCommand() const
{
	return command;
}

std::vector<std::string>	Message::getParams() const
{
	return parametrs;
}

Message::~Message()
{}
