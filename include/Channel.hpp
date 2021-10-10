#pragma once

class User;

#include <string>
#include <vector>
#include <map>
#include <ctime>
#include "User.hpp"
#include "utils.hpp"

#define PRIVATE		0b000001
#define SECRET		0b000010
#define MODERATED	0b000100
#define INVITEONLY	0b001000
#define TOPICSET	0b010000
#define NOMSGOUT	0b100000

class Channel
{
	private:
		std::string						name;
		std::vector<const User *>		operators;
		std::vector<const User *>		speakers;
		std::string						pass;
		unsigned short					userLimit;
		std::vector<std::string>		banMasks;
		unsigned char					flags;
		std::vector<const User *>		users;
		std::string						topic;
		std::vector<const User *>		invitedUsers;

		Channel();
		Channel(const Channel& copy);
		Channel	&operator=(const Channel& other);
		bool							isBanned(const std::string &mask, const std::string &prefix);
		void							sendInfo(const User &user);
	public:
		Channel(const std::string &name, const User &creator, const std::string &pass = "");
		virtual ~Channel();

		const std::string				&getName() const;
		const std::string				&getTopic() const;
		unsigned char					getFlags() const;
		void							setTopic(const User &user, const std::string &topic);
		void							setLimit(unsigned short limit);
		void							setKey(const User &user, const std::string &key);

		bool							isInvited(const User &user) const;
		bool							isOperator(const User &user) const;
		bool							isSpeaker(const User &user) const;
		bool							isEmpty() const;
		bool							containsNickname(const std::string &nickname) const;

		void							connect(const User &user, const std::string &key);
		void							setFlag(unsigned char flag);
		void							removeFlag(unsigned char flag);
		void							sendMessage(const std::string &message, const User &from, bool includeUser) const;
		void							invite(const User &user, const User &receiver);
		void							addOperator(const User &user);
		void							removeOperator(const User &user);
		void							addSpeaker(const User &user);
		void							removeSpeaker(const User &user);
		void							addBanMask(const std::string &mask);
		void							removeBanMask(const std::string &mask);
		void							displayTopic(const User &user);
		void							displayNames(const User &user);
		void							disconnect(const User &user);
		void							removeInvited(const User &user);
		void							displayChanInfo(const User &user);
		std::string						getFlagsAsString() const;
};
