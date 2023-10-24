
NAME = WebServ

SRC_DIR = $(sort $(dir $(wildcard srcs/*/))) srcs/
OBJ_DIR = obj/
INC_DIR = include/
TARGET_DIR = ./
CXX = c++
C98FLAG = -std=c++98
EXTRA =-Wall -Werror -Wextra
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
	main.cpp		\
	Logger.cpp		\
	TimeUtils.cpp	

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
			make all FLAGS="-g -fsanitize=address"

re: 		fclean all

# Dir creation
$(OBJ_DIR) $(TARGET_DIR):
	@mkdir -p $@

-include $(DEPS)

.PHONY: all clean fclean re debug

vpath %.cpp $(SRC_DIR)
override FLAGS += $(C98FLAG) $(EXTRA) $(DEP) $(INC_DIR:%=-I%)
