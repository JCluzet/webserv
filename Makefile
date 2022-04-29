#  |  |  ___ \    \  |         |
#  |  |     ) |  |\/ |   _  |  |  /   _ 
# ___ __|  __/   |   |  (   |    <    __/ 
#    _|  _____| _|  _| \__,_| _|\_\ \___|
#                              by jcluzet
################################################################################
#                                     CONFIG                                   #
################################################################################

NAME        := webserv
CC         := c++

ifeq ($(UNAME), Darwin)
FLAGS    = -Wall -Wextra -Werror -std=c++98#-fsanitize=address -g3
endif
ifeq ($(UNAME), Linux)
FLAGS    = -Wall -Wextra -Werror -std=c++98#-fsanitize=address -g3
endif

CLR_RMV		:= \033[0m
RED		    := \033[1;31m
GREEN		:= \033[1;32m
WHITE		:= \033[1;37m
YELLOW		:= \033[1;33m
BLUE		:= \033[1;34m
CYAN 		:= \033[1;36m
 
################################################################################
#                                 PROGRAM'S SRCS                               #
################################################################################

SRCS        :=      srcs/utils.cpp \
                          srcs/parser.cpp \
                          srcs/server.cpp \
						srcs/autoindex.cpp \
						srcs/Response.cpp \
                          
OBJS        := ${SRCS:.cpp=.o}

.cpp.o:
	@${CC} ${FLAGS} -c $< -o ${<:.cpp=.o} 
	@printf "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
	@printf "$(BLUE)[⊛] =>$(WHITE) $< [ $(GREEN)✔️$(WHITE) ]"

################################################################################
#                                  Makefile  objs                              #
################################################################################

RM		    := rm -f

${NAME}:	${OBJS} 
			@printf "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
			@echo "$(BLUE)[⊛] => $(WHITE)Compilation of ${YELLOW}$(NAME) ${CLR_RMV}..."
			@${CC}  -o ${NAME} ${OBJS} ${FLAGS} -lpthread
			@printf "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
			@printf "$(GREEN)[⊛] => $(WHITE)$(NAME) created [ $(GREEN)✔️$(WHITE) ]\n"

			
all:		${NAME}

bonus:		all

clean:
			@ ${RM} *.o */*.o */*/*.o
			@ printf "$(RED)[⊛] => $(WHITE)Delete $(NAME).o $(CLR_RMV)objs"

fclean:		clean
			@ ${RM} ${NAME}
			@printf "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
			@ echo "$(RED)[⊛] => $(WHITE)Delete $(NAME) $(CLR_RMV)binary"

re:			fclean all

.PHONY:		all clean fclean re


