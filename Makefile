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
FLAGS    := -Wall -Wextra -Werror -std=c++98

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
			@echo "\n$(BLUE)[⊛] => $(WHITE)Compilation of ${YELLOW}$(NAME) ${CLR_RMV}..."
			${CC} ${FLAGS} -o ${NAME} ${OBJS}
			@echo "$(GREEN)[⊛] => $(WHITE)$(NAME) created [ $(GREEN)✔️$(WHITE) ]"

			
all:		${NAME}

bonus:		all

clean:
			@ ${RM} *.o */*.o */*/*.o
			@ echo "$(RED)Deleting $(CYAN)$(NAME) $(CLR_RMV)objs"

fclean:		clean
			@ ${RM} ${NAME}
			@ echo "$(RED)Deleting $(CYAN)$(NAME) $(CLR_RMV)binary\n"

re:			fclean all

.PHONY:		all clean fclean re


