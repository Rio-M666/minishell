#ifndef MINISHELL_H
# define MINISHELL_H

# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <termios.h>
# include <unistd.h>

/* ========== DATA STRUCTURES ========== */

typedef struct s_shell
{
	char	**envp;
	int		last_status;
}	t_shell;

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_REDIR_HEREDOC,
}	t_token_type;

typedef enum e_quote_type
{
	QUOTE_NONE,
	QUOTE_SINGLE,
	QUOTE_DOUBLE,
}	t_quote_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	t_quote_type	quote;
	struct s_token	*next;
}	t_token;

typedef enum e_redir_type
{
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_HEREDOC
}	t_redir_type;

typedef struct s_redirect
{
	t_redir_type		type;
	char				*filename;
	int					heredoc_fd;
	int					expand_heredoc;
	struct s_redirect	*next;
}	t_redirect;

typedef struct s_cmd
{
	char			**args;
	t_redirect		*redir_list;
	struct s_cmd	*next;
}	t_cmd;

/* ========== GLOBAL VARIABLE ========== */

extern volatile sig_atomic_t	g_signal;

/* ========== UTILS (utils.c) ========== */

size_t	ft_strlen(const char *str);
char	*ft_strdup(const char *s);
char	*ft_strchr(const char *str, int c);
char	*ft_substr(char const *s, unsigned int start, size_t len);
char	*ft_strjoin(char const *s1, char const *s2);
char	**ft_split(char const *s, char c);
void	ft_putstr_fd(char *s, int fd);
int		ft_strcmp(const char *s1, const char *s2);
int		ft_isalpha(char c);
int		ft_isalnum(int c);
int		is_space(char c);
void	free_array(char **arr);

/* ========== TOKENIZER (tokenizer.c) ========== */

t_token	*tokenize(char *input);
void	free_tokens(t_token *tokens);

/* ========== PARSER (parser.c) ========== */

t_cmd	*parse(t_token *tokens);
void	free_pipeline(t_cmd *pipeline);

/* ========== EXPANDER (expander.c) ========== */

void	expand_tokens(t_token *tokens, t_shell *shell);
char	*expand_variables(char *str, t_shell *shell);

/* ========== EXECUTE (execute.c) ========== */

int		execute_pipeline(t_cmd *pipeline, t_shell *shell);
int		process_heredocs(t_cmd *pipeline, t_shell *shell);
void	cleanup_heredocs(t_cmd *pipeline);

/* ========== SIGNALS (main.c) ========== */

void	setup_signals_interactive(void);
void	setup_signals_child(void);
void	handle_sigint_heredoc(int sig);

#endif
