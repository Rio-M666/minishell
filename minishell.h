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

// ===================================================================
// 構造体定義
// ===================================================================

typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}					t_list;

typedef struct s_shell
{
	char	**envp;
	int		last_status;
}			t_shell;

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_REDIR_HEREDOC,
}			t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}					t_token;

typedef enum e_redir_type
{
	REDIR_IN,		// <
	REDIR_OUT,		// >
	REDIR_APPEND,	// >>
	REDIR_HEREDOC	// <<
}					t_redir_type;

typedef struct s_redirect
{
	t_redir_type		type;
	char				*filename;
	struct s_redirect	*next;
}					t_redirect;

typedef struct s_cmd
{
	char				**args;
	t_redirect			*redir_list;
	struct s_cmd		*next;
}					t_cmd;

// ===================================================================
// プロトタイプ宣言
// ===================================================================

// --- execute.c ---
int		execute_with_args(char **args);

// --- path.c ---
char	*get_command_path(char *cmd);

// --- utils.c ---
char	**ft_split(char const *s, char c);
char	*ft_strjoin(char const *s1, char const *s2);
void	free_array(char **arr);
char	*ft_strchr(const char *str, int c);
char	*ft_strdup(const char *s);
void	ft_putstr_fd(char *s, int fd);
char	*ft_substr(char const *s, unsigned int start, size_t len);
size_t	ft_strlen(const char *str);

// --- tokenizer.c ---
t_token	*tokenize(char *input);
void	free_tokens(t_token *tokens);
void	print_tokens(t_token *tokens);
int		handle_syntax_error(char *value);

// --- tokenizer_utils.c ---
t_token	*create_token(t_token_type type, char *value);
void	add_token(t_token **head, t_token *new_token);

// --- parser.c ---
t_cmd	*parse(t_token *tokens);

// --- parser_free.c ---
void	free_pipeline(t_cmd *pipeline);

// --- parser_debug.c ---
void	print_pipeline(t_cmd *pipeline);

// --- parser_utils.c ---
t_cmd		*create_cmd(void);
void		add_cmd_back(t_cmd **list, t_cmd *new_cmd);
t_redirect	*create_redirect(t_redir_type type, char *filename);
void		add_redir_back(t_redirect **list, t_redirect *new_redir);
char		**convert_list_to_array(t_list *list);

// --- parser_list_utils.c ---
t_list	*ft_lstnew(void *content);
void	ft_lstadd_back(t_list **lst, t_list *new);
int		ft_lstsize(t_list *lst);
void	ft_lstclear(t_list **lst, void (*del)(void *));

#endif