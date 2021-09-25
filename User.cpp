#include "User.hpp"

User::User(int sockfd) :
sockfd(sockfd), role(client)
{
	(void)role;
}

User::~User()
{}

int		User::getSockfd() const
{
	return sockfd;
}

bool	newLine(char c)
{
	return c == '\n';
}

bool	notNewLine(char c)
{
	return !(newLine(c));
}

std::queue<std::string>	split(const std::string &s)
{
	std::queue<std::string>	ret;
	std::string::const_iterator	i = s.begin();
	while(i != s.end())
	{
		i = std::find_if(i, s.end(), notNewLine);
		std::string::const_iterator	j = std::find_if(i, s.end(), newLine);
		if (i != s.end())
		{
			if (j != s.end())
				++j;
			ret.push(std::string(i, j));
			if (j != s.end())
				--j;
			i = j;
		}
	}
	return ret;
}

void	User::readMessage()
{
	std::string	text;
	if (messages.size() > 0)
		text = messages.front();
	char buffer[100];
	int bytesRead;
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
	while ((bytesRead = read(sockfd, buffer, 99)) > 0)
	{
		buffer[bytesRead] = 0;
		text += buffer;
		buffer[0] = 0;
	}
	messages = split(text);
}

int		User::hadleMessages()
{
	while (messages.size() > 0 && messages.front()[messages.front().size() - 1] == '\n')
	{
		std::string	message = messages.front();
		messages.pop();
		std::cout << "The message was: " << message;
		// handle
		if (message == "end\n")
		{
			std::string response = "Good talking to you\n";
			send(sockfd, response.c_str(), response.size(), 0);
			// Close the connections
			close(sockfd);
			return (-1);
		}
		else if (message == "help\n")
			send(sockfd, "Ne pomogu!\n", 11, 0);
	}
	return (0);
}
