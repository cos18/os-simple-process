NAME		= paging

SRCDIR		= ./srcs/
INCDIR		= ./includes/

SRCNAMES	= $(shell ls $(SRCDIR) | grep -E ".+\.cpp")
OBJS		= $(SRCS:.cpp=.o)

BUILDDIR	= ./build/
BUILDOBJS	= $(addprefix $(BUILDDIR), $(SRCNAMES:.cpp=.o))

CC			= g++
CCFLAG		= -g -Wall -Wextra -Werror -Wno-c++11-extensions
RM			= rm -f

all:			$(BUILDDIR) $(NAME)

$(BUILDDIR):
				mkdir -p $(BUILDDIR)

$(BUILDDIR)%.o:	$(SRCDIR)%.cpp
				$(CC) $(CCFLAG) -I$(INCDIR) -o $@ -c $<

$(NAME):		$(BUILDOBJS)
				$(CC) $(CCFLAG) -I$(INCDIR) -o $(NAME) $(BUILDOBJS)

clean:
				$(RM) $(BUILDOBJS)
				rm -rf $(BUILDDIR)
				rm -rf .TMP_FILE*

fclean:			clean
				$(RM) $(NAME)

re:				fclean all

.PHONY:			all clean fclean re