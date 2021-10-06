#ifndef CHANNEL_HPP
# define CHANNEL_HPP

class User;

# include <string>
# include <vector>
# include <map>
# include <ctime>
# include "User.hpp"

# define PRIVATE	0b00001
# define SECRET		0b00010
# define MODERATED	0b00100
# define INVITEONLY	0b01000
# define TOPICSET	0b10000

class Channel
{
	private:
		std::string						name;
		std::vector<const User *>		operators;
		std::string						pass;
		unsigned short					userLimit;
		std::vector<std::string>		banMasks;
		unsigned char					flags;
		std::map<const User *, time_t>	users;
		std::string						topic;
		std::vector<const User *>		invitedUsers;

		Channel();
		Channel(const Channel& copy);
		Channel	&operator=(const Channel& other);
	public:
		Channel(const std::string &name, const User &creator, const std::string &pass = "");
		virtual ~Channel();

		const std::string				&getName() const;
		void							setTopic(const std::string &topic);

		bool							isInvited(const User &user);

		void							connect(const User &user, const std::string &key);
		void							setFlag(unsigned char flag);
		void							removeFlag(unsigned char flag);
		void							sendMessage(const std::string &message, const User &from);
};

#endif
