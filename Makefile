NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror -g -I$(CURDIR)
INCLUDES = -I./includes
LDFLAGS = -lreadline


SRCS = main.c execute.c utils.c path.c pipe.c signal_handlers.c heredoc.c
SRCS += tokenizer/tokenizer.c \
		tokenizer/tokenizer_token_utils.c \
		tokenizer/tokenizer_parse_utils.c \
		tokenizer/tokenizer_quote_utils.c \
		tokenizer/tokenizer_error.c
SRCS += expander/expander.c \
		expander/expander_variable.c \
		expander/expander_utils.c
SRCS += parser/parser.c \
		parser/parser_utils.c \
		parser/parser_list_utils.c \
		parser/parser_free.c \
		parser/parser_debug.c
SRCS += env/env_manager.c
SRCS += builtins/ft_cd.c \
		builtins/ft_echo.c \
		builtins/ft_pwd.c \
		builtins/ft_export.c \
		bulitins/ft_unset.c \
		builtins/ft_env.c \
		builtins/ft_exit.c


# Object files
OBJS = $(SRCS:.c=.o)


all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(NAME)
	@echo "$(GREEN)✓ minishell compiled successfully$(RESET)"

%.o: %.c
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	@echo "$(GREEN)✓ Compiled: $<$(RESET)"

clean:
	@rm -f $(OBJS)
	@echo "$(RED)✗ Object files removed$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)✗ $(NAME) removed$(RESET)"

re: fclean all

.PHONY: all clean fclean re
