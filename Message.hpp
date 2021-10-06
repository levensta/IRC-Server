#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <string>
# include <vector>
# include "utils.hpp"

class Message
{
	private:
		std::string						prefix;
		std::string						command;
		std::vector<std::string>		parameters;
	public:
		Message(std::string &str);
		virtual ~Message();
		const std::string				&getPrefix() const;
		const std::string				&getCommand() const;
		const std::vector<std::string>	&getParams() const;
};

#endif
