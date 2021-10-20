NAME= ircserv

SOURCEFILES=	main.cpp \
				Server.cpp \
				User.cpp \
				Message.cpp \
				utils.cpp \
				sendError.cpp \
				sendReply.cpp \
				Channel.cpp \
				History.cpp \
				commands/channelCommands.cpp \
				commands/otherCommands.cpp \
				commands/registrationCommands.cpp \
				commands/userCommands.cpp

LIBJSONFOLDER = json-parser
LIBJSONFLAGS = -ljson -L ./$(LIBJSONFOLDER) -I ./$(LIBJSONFOLDER)/src

SOURCEFOLDER= srcs/

OSOURCEFOLDER= objects/

INCLUDEFOLDER= include/

SOURCE= $(addprefix $(SOURCEFOLDER), $(SOURCEFILES))

OSOURCE= $(addprefix $(OSOURCEFOLDER), $(SOURCEFILES:.cpp=.o))

all: libjson $(NAME)

$(OSOURCEFOLDER):
	mkdir objects
	mkdir objects/commands

$(OSOURCEFOLDER)%.o: $(SOURCEFOLDER)%.cpp
	clang++ -Wall -Werror -Wextra -c $< -o $@ -std=c++98 -I $(INCLUDEFOLDER) -I ./$(LIBJSONFOLDER)/src

libjson:
	$(MAKE) -C $(LIBJSONFOLDER) all

$(NAME): $(OSOURCEFOLDER) $(OSOURCE)
	clang++ $(OSOURCE) -o $(NAME) $(LIBJSONFLAGS)

clean:
	$(MAKE) -C $(LIBJSONFOLDER) clean
	rm -rf $(OSOURCEFOLDER)

fclean: clean
	$(MAKE) -C $(LIBJSONFOLDER) fclean
	rm -rf $(NAME)

re: fclean all

.PHONY: clean fclean re all
