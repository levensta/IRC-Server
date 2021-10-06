NAME= ircserv

SOURCE=	main.cpp \
		Server.cpp \
		User.cpp \
		Message.cpp \
		utils.cpp \
		sendError.cpp \
		sendReply.cpp \
		Channel.cpp

OSOURCEFOLDER= objects/

OSOURCE= $(addprefix $(OSOURCEFOLDER), $(SOURCE:.cpp=.o))

all: $(NAME)

$(OSOURCEFOLDER):
	mkdir objects

$(OSOURCEFOLDER)%.o: %.cpp
	clang++ -g -Wall -Werror -Wextra -c $< -o $@

$(NAME): $(OSOURCEFOLDER) $(OSOURCE)
	clang++ -g $(OSOURCE) -o $(NAME)

clean:
	rm -rf $(OSOURCEFOLDER)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: clean fclean re all
