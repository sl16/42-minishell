# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aulicna <aulicna@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/23 14:30:35 by aulicna           #+#    #+#              #
#    Updated: 2023/11/26 19:25:26 by aulicna          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

SRC = main.c \
						prompt.c \
						ft_split_minishell.c

OBJ = $(SRC:.c=.o)

HEADER = incl/minishell.h

CFLAGS = -Wall -Werror -Wextra -g

GCC = gcc

LIBFTPRINTF = libftprintf

all: libs $(NAME)
	@echo "minishell executable ready ✅"

.c.o:
	$(GCC) $(CFLAGS) -c $< -o $@

libs:
	@make -C $(LIBFTPRINTF)
	@echo "libprintf library ready ✅"

$(NAME): $(OBJ) $(HEADER)
	$(GCC) $(CFLAGS) -L $(LIBFTPRINTF) -o $@ $^ -lreadline -lftprintf

clean:
	@rm -f $(OBJ)
	@make clean -C $(LIBFTPRINTF)

fclean:
	@rm -f $(OBJ)
	@rm -f $(NAME)
	@make fclean -C $(LIBFTPRINTF)

re: fclean all

.PHONY: all clean fclean re