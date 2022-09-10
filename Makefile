##################################################
#			GLOBAL VARIABLES DEFINITION			 #
##################################################

TARGET	= ft_ping

CFLAGS	= -Wall -Wextra -g #-Werror
CC		= gcc

INC 	= -I ./include -I ./libft

SRCDIR	= ./src/
SRC		= ft_ping.c utils.c

OBJDIR	= ./objs/
OBJS	= ${addprefix ${OBJDIR}, ${SRC:.c=.o}}

LIBFT	= libft/libft.a

##################################################
#				COMPILATION RULES				 #
##################################################

${OBJDIR}%.o: ${SRCDIR}%.c
	${CC} ${CFLAGS} -c $< ${INC} -o $@

${TARGET}: ${OBJDIR} ${OBJS} ${LIBFT}
	${CC} ${OBJS} ${LIBFT} -o $@

${OBJDIR}:
	mkdir -p ${OBJDIR}

${LIBFT}:
	${MAKE} -C libft

##################################################
#  				   USUAL RULES					 #./
##################################################

all: ${TARGET}

clean:
	rm -rf ${OBJDIR}
	${MAKE} clean -C libft

fclean: clean
	rm -rf ${TARGET}
	${MAKE} fclean -C libft

re: fclean all

.PHONY:	all clean fclean re
