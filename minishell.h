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
typedef struct s_shell
{
	char **envp; // 今後のため環境変数など保持予定
	int	last_status;
}		t_shell;
int		execute_with_args(char **args);
char	**ft_split(char const *s, char c);
char	*ft_strjoin(char const *s1, char const *s2);
void	free_array(char **arr);
char	*ft_strchr(const char *str, int c);
char	*ft_strdup(const char *s);
char	*get_command_path(char *cmd);
void	ft_putstr_fd(char *s, int fd);
#endif