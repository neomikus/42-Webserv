NAME		:=	webserv
OBJ_DIR		:=	obj/
SRC_PATH	:=	src/
CC			:=	c++
CPPFLAGS = -Wall -Wextra -Werror -Wshadow -std=c++98 -Iinclude

# SRCS #
MAIN		:= main
FILES		:= File
CONTEXT		:= Location Server
UTILS	 	:= httpUtils strUtils intUtils teapot
SOCKETS		:= Socket connections
REQUESTS	:= Request Get Post Delete

PLAIN_SRCS 	:=	$(addsuffix .cpp, $(addprefix main/,	$(MAIN))) \
				$(addsuffix .cpp, $(addprefix context/,	$(CONTEXT))) \
				$(addsuffix .cpp, $(addprefix files/,	$(FILES))) \
				$(addsuffix .cpp, $(addprefix utils/,	$(UTILS))) \
				$(addsuffix .cpp, $(addprefix sockets/,	$(SOCKETS))) \
				$(addsuffix .cpp, $(addprefix requests/,	$(REQUESTS)))


SRCS 		:=	$(addprefix $(SRC_PATH), $(PLAIN_SRCS))
OBJS 		:=	$(addprefix $(OBJ_DIR), $(PLAIN_SRCS:.cpp=.o))

# ASCII COLORS #
BLACK=\033[0;30m
RED=\033[0;31m
#RED=\033[38;5;208m
GREEN=\033[0;32m
YELLOW=\033[0;33m
BLUE=\033[0;34m
MAG=\033[0;35m
CYAN=\033[38;5;6m
#WHITE=\033[1;37m
WHITE=\033[38;5;255m
PAPYRUS=\033[38;5;223m
PINK=\033[38;5;213m
END=\033[0m
WHITEB=\033[48;5;255m
CYANB=\033[48;5;6m
PINKB=\033[48;5;213m

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CPPFLAGS) $(OBJS) $(OBJS_FAGS) -o $(NAME)

$(OBJ_DIR)%.o: $(SRC_PATH)%.cpp
	@mkdir -p $(@D)
	@$(CC) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@rm -fr $(OBJ_DIR)

fclean:
	@rm -fr $(OBJ_DIR)
	@rm -f $(NAME)
	
re: fclean all

.PHONY: all clean fclean re