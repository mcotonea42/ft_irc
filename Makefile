PROGRESS = \033[33m⏳\033[0m
BROOM = \033[33m🧹\033[0m
SUCCESS = \033[32m✔\033[0m

CXX = c++
INCLUDES = -I./includes
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 $(INCLUDES)

NAME = ircserv

SRC_FOLDER = ./sources
SRC = ./sources/Server.cpp			\
	  ./sources/Client.cpp			\
	  ./sources/NickCommand.cpp		\
	  ./sources/UserCommand.cpp		\
	  ./sources/PassCommand.cpp		\
	  ./sources/PingCommand.cpp		\
	  ./sources/ModeCommand.cpp		\
	  ./sources/JoinCommand.cpp		\
	  ./sources/Channel.cpp			\
	  ./sources/PrivmsgCommand.cpp	\
	  ./sources/PartCommand.cpp		\
	  ./sources/KickCommand.cpp		\
	  ./sources/QuitCommand.cpp		\
	  ./sources/TopicCommand.cpp	\
	  ./sources/InviteCommand.cpp	\
	  ./sources/Bot.cpp				\
	  ./sources/main.cpp

OBJ_DIR =./obj
OBJS = $(SRC:$(SRC_FOLDER)%.cpp=$(OBJ_DIR)%.o)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)



$(OBJ_DIR)/%.o: $(SRC_FOLDER)/%.cpp | $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)


clean:
	@echo "\n$(BROOM) Cleaning !!! $(BROOM)\n"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re