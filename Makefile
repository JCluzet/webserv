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

# ifeq ($(UNAME), Darwin)
FLAGS    = -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g3
# endif
# ifeq ($(UNAME), Linux)
# FLAGS    = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3
# endif

CLR_RMV		:= \033[0m
RED		    := \033[1;31m
GREEN		:= \033[1;32m
WHITE		:= \033[1;37m
YELLOW		:= \033[1;33m
BLUE		:= \033[1;34m
CYAN 		:= \033[1;36m
GRAY		:= \033[1;30m
CLEAR_LINE := \33[2K\r
 
################################################################################
#                                 PROGRAM'S SRCS                               #
################################################################################

SRCS        :=      srcs/utils.cpp \
                          srcs/server.cpp \
						srcs/autoindex.cpp \
						srcs/Response.cpp \
						srcs/Config.cpp \
						srcs/cgi.cpp \
						srcs/Client.cpp \
						srcs/main.cpp 
                          
OBJS        := ${SRCS:.cpp=.o}

.cpp.o:
	@${CC} ${FLAGS} -c $< -o ${<:.cpp=.o} 
	@printf "$(CLEAR_LINE)"
	@printf "$(WHITE)[$(GRAY)⊛$(WHITE)] => $< $(GREEN)✔️$(WHITE)"

################################################################################
#                                  Makefile  objs                              #
################################################################################

RM		    := rm -f

${NAME}:	${OBJS} 
			@printf "$(CLEAR_LINE)"
			@echo "$(WHITE)[$(GREEN)⊛$(WHITE)] => Compilation Success $(GREEN)✔️$(WHITE)"
			@echo "$(WHITE)[$(GRAY)⊛$(WHITE)] => $(WHITE)Linkage of ${GRAY}$(NAME)${CLR_RMV}..."
			@${CC}  -o ${NAME} ${OBJS} ${FLAGS} -lpthread
			@printf "$(CLEAR_LINE)"
			@printf "$(WHITE)[$(GREEN)⊛$(WHITE)] $(WHITE)=> $(GREEN)$(NAME)$(WHITE) created $(GREEN)✔️$(WHITE)\n"

			
all:		${NAME}

bonus:		all

clean:
			@ ${RM} *.o */*.o */*/*.o
			@ printf "$(RED)[⊛] => $(WHITE)Delete $(NAME).o $(CLR_RMV)objs"

fclean:		clean
			@ ${RM} ${NAME}
			@printf "$(CLEAR_LINE)"
			@ echo "$(WHITE)[$(RED)⊛$(WHITE)] => $(WHITE)Delete $(NAME) $(CLR_RMV)binary"

re:			fclean all

.PHONY:		all clean fclean re
