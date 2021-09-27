#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <string>
# include <vector>
# include "utils.hpp"

class Message
{
	std::string						prefix;
	std::string						command;
	std::vector<std::string>		parametrs;
	public:
		Message(std::string &str);
		virtual ~Message();
		std::string					getPrefix() const;
		std::string					getCommand() const;
		std::vector<std::string>	getParams() const;
};

#endif
