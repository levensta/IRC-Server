NAME= ircserv

SOURCEFILES=	main.cpp \
				Server.cpp \
				User.cpp \
				Message.cpp \
				utils.cpp \
				sendError.cpp \
				sendReply.cpp \
				Channel.cpp \
				commands/channelCommands.cpp \
				commands/otherCommands.cpp \
				commands/registrationCommands.cpp \
				commands/userCommands.cpp

SOURCEFOLDER= srcs/

OSOURCEFOLDER= objects/

INCLUDEFOLDER= include/

SOURCE= $(addprefix $(SOURCEFOLDER), $(SOURCEFILES))

OSOURCE= $(addprefix $(OSOURCEFOLDER), $(SOURCEFILES:.cpp=.o))

all: $(NAME)

$(OSOURCEFOLDER):
	mkdir objects
	mkdir objects/commands

$(OSOURCEFOLDER)%.o: $(SOURCEFOLDER)%.cpp
	clang++ -Wall -Werror -Wextra -c $< -o $@ -std=c++11 -I $(INCLUDEFOLDER)

$(NAME): $(OSOURCEFOLDER) $(OSOURCE)
	clang++ $(OSOURCE) -o $(NAME)

clean:
	rm -rf $(OSOURCEFOLDER)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: clean fclean re all
