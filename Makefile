# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bmarilli <bmarilli@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/04/13 20:44:46 by bmarilli          #+#    #+#              #
#    Updated: 2022/04/15 23:59:54 by bmarilli         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME =		webserv

CXX =		clang++

INC_DIR =	$(shell find includes -type d)


#Compilation flag
CPPFLAGS = -Wall -Wextra -Werror -std=c++98

OBJ_DIR =	objs
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:%.cpp=%.o))

# Colors

_GREY=	$'\033[30m
_RED=	$'\033[31m
_GREEN=	$'\033[32m
_YELLOW=$'\033[33m
_BLUE=	$'\033[34m
_PURPLE=$'\033[35m
_CYAN=	$'\033[36m
_WHITE=	$'\033[37m

all:			$(NAME)
	

select:
	clang++ -Iincludes srcs/ServerApi.cpp srcs/ServerSelect.cpp  main2.cpp

poll:
	clang++ -Iincludes srcs/ServerApi.cpp srcs/ServerPoll.cpp main3.cpp

$(NAME): 		$(INC_DIR) $(OBJ)