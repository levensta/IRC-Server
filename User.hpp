#ifndef USER_HPP
# define USER_HPP

# include <string>
# include <iostream>
# include <sstream>
# include <unistd.h>
# include <vector>
# include <queue>
# include <sys/socket.h>
# include <algorithm>
# include <fcntl.h>
# include "utils.hpp"
# include "Message.hpp"

enum Role
{
	client,
	ChanelOperator,
	IrcOperator,
};

class User
{
	private:
		int							sockfd;
		std::queue<std::string>		messages;
		Role						role;
	public:
		User(int sockfd);
		~User();
		int							getSockfd() const;
		void						readMessage();
		std::vector<std::string>	parseCommand();
		int							hadleMessages();
};

#endif
