.PHONY: all re fclean clean

NAME = bot

CC = clang++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic -Wno-unused

OBJDIR = .obj
SRCDIR = src
HEADERS = Bot.hpp Socket.hpp Message.hpp utils.hpp
SOURCES = Bot.cpp Socket.cpp main.cpp Message.cpp utils.cpp


LIBJSONFOLDER = ../json-parser
LIBJSONFLAGS = -ljson -L ./$(LIBJSONFOLDER) -I ./$(LIBJSONFOLDER)/src

OBJECTS = $(addprefix ${OBJDIR}/, $(SOURCES:.cpp=.o))
FULL_HEADERS = $(addprefix ${SRCDIR}/, ${HEADERS})

all: objdir libjson $(NAME)

objdir:
	@if ! [ -d ${OBJDIR} ] ; then mkdir ${OBJDIR} ; fi

libjson:
	$(MAKE) -C $(LIBJSONFOLDER) all

$(NAME): $(OBJECTS)
	$(CC) $(CPPFLAGS) $^ $(LIBJSONFLAGS) -o $@

${OBJDIR}/%.o: ${SRCDIR}/%.cpp ${FULL_HEADERS}
	$(CC) $(CPPFLAGS) -c $< -I ./$(LIBJSONFOLDER)/src -o $@  

clean:
	@rm -rf ${OBJDIR}

fclean: clean
	@rm -rf $(NAME)

re: fclean all
