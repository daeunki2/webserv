# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: locherif <locherif@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/23 19:31:22 by daeunki2          #+#    #+#              #
#    Updated: 2025/11/01 20:58:11 by locherif         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -I $(INC_DIR)

OBJ_DIR = obj
SRC_DIR = src
INC_DIR = inc

SRCS = $(SRC_DIR)/test.cpp \
        $(SRC_DIR)/request_parser/http_request.cpp \
        $(SRC_DIR)/request_parser/request_parser.cpp \
        $(SRC_DIR)/config_parser/config_parser.cpp  \
        $(SRC_DIR)/config_parser/config_parser_utils.cpp \

		

        
        
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

# --- 1. $(OBJ_DIR): 타겟을 제거합니다 (아래 규칙에서 처리됨) ---

$(NAME): $(OBJS)
	@echo "Building $(NAME) 🛠️"
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "Build Complete! ✅"

# --- 2. 객체 파일 생성 규칙 수정 (하위 디렉토리 자동 생성) ---
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
    # $@는 목표 파일 이름(예: obj/request_parser/http_request.o)입니다.
    # $(dir $@)는 해당 파일의 디렉토리 경로(예: obj/request_parser/)를 반환합니다.
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