# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: daeunki2 <daeunki2@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/23 19:31:22 by daeunki2          #+#    #+#              #
#    Updated: 2025/11/26 11:18:43 by daeunki2         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv
CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98 -g\
		-I $(SRC_DIR)/config_parser \
		-I $(SRC_DIR)/etc \
		-I $(SRC_DIR)/Client \
		-I $(SRC_DIR)/server_manager

OBJ_DIR = obj
SRC_DIR = src
INC_DIR = inc


SRCS = $(SRC_DIR)/main.cpp \
		$(SRC_DIR)/config_parser/config_parser.cpp \
		$(SRC_DIR)/config_parser/Location.cpp \
		$(SRC_DIR)/config_parser/Server.cpp \
		$(SRC_DIR)/server_manager/Server_Manager.cpp \
		$(SRC_DIR)/etc/Error.cpp \
		$(SRC_DIR)/etc/Logger.cpp \
		$(SRC_DIR)/etc/Utils.cpp \
        $(SRC_DIR)/Client/Client.cpp \
		$(SRC_DIR)/Client/request_parser.cpp \
		$(SRC_DIR)/Client/http_request.cpp \
		$(SRC_DIR)/Client/Response_Builder.cpp \

OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	@echo "Building $(NAME) 🛠️"
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "Build Complete! ✅"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning object files 🧹"
	@rm -rf $(OBJ_DIR)
	@echo "cleaning ✅"

fclean: clean
	@echo "Cleaning Executable 🧹"
	@rm -f $(NAME)
	@echo "fclean ✅"

re: fclean all

.PHONY: all clean fclean re