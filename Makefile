NAME := philo
CC := cc
# CFLAGS := -Wall -Wextra -Werror 
CFLAGS := -Wall -Wextra -Werror -fsanitize=address -g
SRCS := philosophers.c ft_atoi.c
OBJS := $(patsubst %.c, %.o, $(SRCS))

all: $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

# %.o: %.c
# 	$(CC) $(CFLAGS) -c $< -o $@

clean :
	rm -f $(OBJS)

fclean : clean
	rm -f $(NAME)

re : fclean 
	make $(NAME)

.PHONY: clean fclean re all