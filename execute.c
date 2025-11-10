#include "minishell.h"

int	execute_with_args(char **args)
{
	pid_t pid;
	int status;
	char *cmd_path;

	if (!args || !args[0])
		return (1);
	cmd_path = get_command_path(args[0]);
	if (!cmd_path)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(args[0], 2);
		ft_putstr_fd(": command not found\n", 2);
		return (127);
	}
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		free(cmd_path);
		return (1);
	}
	if (pid == 0)
	{
		extern char **environ;
		execve(cmd_path, args, environ);

		perror("execve");
		exit(126);
	}
	else
	{
		free(cmd_path);
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			return (WEXITSTATUS(status));
		return (1);
	}
}