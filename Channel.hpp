#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>
# include "User.hpp"

# define PRIVATE	0b00001
# define SECRET		0b00010
# define MODERATED	0b00100
# define INVITEONLY	0b01000
# define TOPICSET	0b10000

class Channel
{
	private:
		std::string					name;
		std::string					mainOperator;
		std::string					operators;
		std::string					pass;
		unsigned short				userLimit;
		std::vector<std::string>	banMasks;
		unsigned char				flags;
		std::vector<User>			users;
		std::string					topic;
	public:
		Channel(const std::string &name, const std::string &creator, const std::string &pass = "");
		virtual ~Channel();

		const std::string			&getName() const;
		void						setTopic(const std::string &topic);

		void						connect(const User &user);
		void						setFlag(unsigned char flag);
		void						removeFlag(unsigned char flag);
		void						sendMessage(const std::string &message, const User &from);
};

#endif
