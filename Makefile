##################################################
#			GLOBAL VARIABLES DEFINITION			 #
##################################################

TARGET	= ft_ping

CFLAGS	= -Wall -Wextra -Werror
CC		= gcc

INC 	= -I ./include -I ./libft

SRCDIR	= ./src/
SRC		= ft_ping.c icmp.c statistics.c args.c utils.c

OBJDIR	= ./objs/
OBJS	= ${addprefix ${OBJDIR}, ${SRC:.c=.o}}

LIBFT	= libft/libft.a

##################################################
#			OUTPUT VARIABLES DEFINITION			 #
##################################################

RED = \e[1;31m
GREEN = \e[1;32m
BLUE = \e[1;34m
PURPLE = \e[0;35m
RESET = \e[0;0m

COMPILE = ${GREEN}Compiling${RESET}
BUILD = ${BLUE}Building${RESET}
CLEAN = ${RED}Cleaning${RESET}

##################################################
#				COMPILATION RULES				 #
##################################################

${OBJDIR}%.o: ${SRCDIR}%.c
	@echo "${COMPILE} $<"
	@${CC} ${CFLAGS} -c $< ${INC} -o $@

${TARGET}: ${OBJDIR} ${OBJS} ${LIBFT}
	@echo "${BUILD} $@"
	@${CC} ${OBJS} ${LIBFT} -o $@
	@echo "${PURPLE}Program was built successfully, have fun playing with ${GREEN}$@"

${OBJDIR}:
	@mkdir -p ${OBJDIR}

${LIBFT}:
	@echo "${BUILD} libft"
	@${MAKE} --no-print-directory -C libft

##################################################
#  				   USUAL RULES					 #
##################################################

all: ${TARGET}

clean:
	@echo "${CLEAN} objects"
	rm -rf ${OBJDIR}

fclean: clean
	@echo "${CLEAN} ${TARGET}"
	rm -rf ${TARGET}
	@echo "${CLEAN} libft"
	@${MAKE} --no-print-directory fclean -C libft

re: fclean all

.PHONY:	all clean fclean re
