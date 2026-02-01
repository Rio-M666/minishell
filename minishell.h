/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: toyamagu <toyamagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 18:53:21 by mrio              #+#    #+#             */
/*   Updated: 2026/01/25 00:28:33 by toyamagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

# define EXIT_CMD_NOT_FOUND		127
# define EXIT_CMD_NOT_EXEC		126
# define EXIT_SIGNAL_BASE		128
# define DEFAULT_FILE_PERMS		0644

typedef struct s_list
{
	void						*content;
	struct s_list				*next;
}								t_list;

typedef struct s_shell
{
	char						**envp;
	int							last_status;
}								t_shell;

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_REDIR_HEREDOC,
}								t_token_type;

typedef enum e_quote_type
{
	QUOTE_NONE,
	QUOTE_SINGLE,
	QUOTE_DOUBLE,
}								t_quote_type;

typedef struct s_token
{
	t_token_type				type;
	char						*value;
	t_quote_type				quote;
	struct s_token				*next;
}								t_token;

typedef enum e_redir_type
{
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_HEREDOC
}								t_redir_type;

typedef struct s_redirect
{
	t_redir_type				type;
	char						*filename;
	int							heredoc_fd;
	int							expand_heredoc;
	struct s_redirect			*next;
}								t_redirect;

typedef struct s_cmd
{
	char						**args;
	t_redirect					*redir_list;
	struct s_cmd				*next;
}								t_cmd;

typedef struct s_quote_state
{
	int							in_single;
	int							in_double;
}								t_quote_state;

typedef struct s_pipe_context
{
	int							pipe_fd[2];
	int							prev_fd[2];
	int							cmd_count;
	t_cmd						*current;
}								t_pipe_context;

extern volatile sig_atomic_t	g_signal;

int								execute_with_args(char **args);

int								execute_pipeline(t_cmd *pipeline,
									t_shell *shell);

int								execute_pipeline_cmd(t_pipe_context *pipe_ctx,
									t_shell *shell);

int								apply_redirections(t_redirect *redir_list);

char							*get_command_path(char *cmd);

char							**ft_split(char const *s, char c);
char							*ft_strjoin(char const *s1, char const *s2);
void							free_array(char **arr);
char							*ft_strchr(const char *str, int c);
char							*ft_strdup(const char *s);
void							ft_putstr_fd(char *s, int fd);
char							*ft_substr(char const *s, unsigned int start,
									size_t len);
size_t							ft_strlen(const char *str);
int								ft_isalpha(char c);
int								ft_isalnum(int c);
int								ft_isdigit(int c);
int								ft_atoi(const char *str);
int								is_space(char c);

t_token							*tokenize(char *input);

t_token							*create_token(t_token_type type, char *value);
void							add_token(t_token **head, t_token *new_token);
void							free_tokens(t_token *tokens);
void							print_tokens(t_token *tokens);

int								handle_syntax_error(char *value);

int								is_special(char c);
void							skip_space(char **str);
char							*get_word(char **str);

t_token							*tokenize_special(char **str);

t_quote_type					get_quote_type(char *raw);
char							*strip_quotes(char *raw);

t_cmd							*parse(t_token *tokens);

int								handle_redirection(t_cmd *cmd,
									t_token **current);

void							free_pipeline(t_cmd *pipeline);

void							print_pipeline(t_cmd *pipeline);

t_cmd							*create_cmd(void);
void							add_cmd_back(t_cmd **list, t_cmd *new_cmd);
t_redirect						*create_redirect(t_redir_type type,
									char *filename);
void							add_redir_back(t_redirect **list,
									t_redirect *new_redir);
char							**convert_list_to_array(t_list *list);

t_list							*ft_lstnew(void *content);
void							ft_lstadd_back(t_list **lst, t_list *new_node);
int								ft_lstsize(t_list *lst);
void							ft_lstclear(t_list **lst, void (*del)(void *));

void							expand_tokens(t_token *tokens, t_shell *shell);

int								get_var_name_len(char *str);
char							*get_env_value(char *var_name, int var_len,
									t_shell *shell);
char							*process_variable(char *str, int *i,
									t_shell *shell, t_quote_state *state);
char							*expand_variables(char *str, t_shell *shell);

void							update_quote_state(char c,
									t_quote_state *state);
char							*append_str(char *result, char *str, int *i,
									int *res_len);

void							handle_sigint_interactive(int sig);
void							setup_signals_interactive(void);
void							setup_signals_child(void);
void							handle_sigint_heredoc(int sig);

int								process_heredocs(t_cmd *pipeline,
									t_shell *shell);
void							cleanup_heredocs(t_cmd *pipeline);

void							setup_signals_heredoc(void);
int								read_heredoc_content(char *marker,
									int expand, t_shell *shell);

/*---env/env_manager.c---*/
int								count_env(char **envp);
char							**init_envp(char **envp);
char							*ft_getenv(char *key, t_shell *shell);
int								ft_setenv(char *key, char *value,
									t_shell *shell);
void							ft_unset_env(char *key, t_shell *shell);

/* --- builtins/ --- */
int								ft_echo(char **args);
int								ft_cd(char **args, t_shell *shell);
int								ft_pwd(void);
int								ft_export(char **args, t_shell *shell);
int								ft_unset(char **args, t_shell *shell);
int								ft_env(t_shell *shell);
int								ft_exit(char **args, t_shell *shell);

int								is_builtin(char *cmd);
int								exec_builtin(t_cmd *cmd, t_shell *shell);

int								ft_strcmp(const char *s1, const char *s2);
int								ft_strncmp(const char *s1, const char *s2,
									size_t n);

void							print_error(char *cmd, char *msg);
void							print_error_arg(char *cmd, char *arg,
									char *msg);

#endif