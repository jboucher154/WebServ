
NAME = WebServ

SRC_DIR = $(sort $(dir $(wildcard srcs/*/))) srcs/
OBJ_DIR = obj/
INC_DIR = include/
TARGET_DIR = ./
CXX = c++
C98FLAG = -std=c++98
EXTRA = -Wall -Wextra -Werror
DEP = -MP -MMD

# Colors

C_RESET = \033[0;39m
GREEN = \033[0;92m
YELLOW = \033[0;93m
BLUE = \033[0;94m
B_MAGENTA = \033[1;35m
CYAN = \033[0;96m

# Sources
SRCS = \
	main.cpp \
	ResponseCodes.cpp \
	Response.cpp \
	Request.cpp \
	Logger.cpp			\
	TimeUtils.cpp		\
	Server.cpp			\
	ft_stoi.cpp			\
	int_to_string.cpp		\
	ServerManager.cpp	\
	Client.cpp			\
	Validator.cpp		\
	file_handling.cpp	\
	is_valid_ip_address.cpp \
	url_encoder.cpp \
	is_all_digit.cpp \
	CgiHandler.cpp			\
	copyCStringArray.cpp	\
	deleteAllocatedCStringArray.cpp	\
	ft_strdup.cpp				\
	Response_directory_listing.cpp


OBJS := $(SRCS:%.cpp=$(OBJ_DIR)%.o)
DEPS = $(SRCS:%.cpp=$(OBJ_DIR)%.d)

###

all:		$(NAME)

$(OBJS):	$(OBJ_DIR)%.o: %.cpp | $(OBJ_DIR)
			@echo "$(GREEN)Compiling: $(BLUE)$<$(C_RESET)"
			@$(CXX) $(FLAGS) -c -o $@ $<

# Target
$(NAME):	$(OBJS) | $(TARGET_DIR)
			@$(CXX) $(FLAGS) -o $@ $^
			@echo "$(B_MAGENTA)\n...$(NAME) compiled successfully...\n$(C_RESET)"

clean:
			rm -rf $(OBJ_DIR)

fclean: clean
			rm -rf $(NAME)

# Debug
debug:
			make fclean
			make all FLAGS="-D GET_DEBUG_LOG=true -g"

debugsan:
			make fclean
			make all FLAGS="-g -fsanitize=address"

re: 		fclean all

# Dir creation
$(OBJ_DIR) $(TARGET_DIR):
	@mkdir -p $@

-include $(DEPS)

.PHONY: all clean fclean re debug debugsan

vpath %.cpp $(SRC_DIR)
override FLAGS += $(C98FLAG) $(EXTRA) $(DEP) $(INC_DIR:%=-I%)
