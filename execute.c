/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 18:46:51 by mrio              #+#    #+#             */
/*   Updated: 2026/01/23 18:46:52 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	child_exec(char *cmd_path, char **args)
{
	extern char	**environ;

	setup_signals_child();
	execve(cmd_path, args, environ);
	perror("execve");
	exit(EXIT_CMD_NOT_EXEC);
}

static int	parent_wait(char *cmd_path, pid_t pid)
{
	int	status;

	free(cmd_path);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	waitpid(pid, &status, 0);
	setup_signals_interactive();
	if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGINT || WTERMSIG(status) == SIGQUIT)
			write(1, "\n", 1);
		return (EXIT_SIGNAL_BASE + WTERMSIG(status));
	}
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (1);
}

static int	check_cmd_error(char *cmd)
{
	struct stat	st;

	if (ft_strchr(cmd, '/'))
	{
		if (stat(cmd, &st) == 0)
		{
			if (S_ISDIR(st.st_mode))
				return (print_error(cmd, "Is a directory"), EXIT_CMD_NOT_EXEC);
			if (access(cmd, X_OK) != 0)
				return (print_error(cmd, "Permission denied"), EXIT_CMD_NOT_EXEC);
		}
		else
			return (print_error(cmd, "No such file or directory"), EXIT_CMD_NOT_FOUND);
	}
	return (print_error(cmd, "command not found"), EXIT_CMD_NOT_FOUND);
}

int	execute_with_args(char **args)
{
	pid_t	pid;
	char	*cmd_path;

	if (!args || !args[0] || !args[0][0])
		return (0);
	cmd_path = get_command_path(args[0]);
	if (!cmd_path)
		return (check_cmd_error(args[0]));
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		free(cmd_path);
		return (1);
	}
	if (pid == 0)
		child_exec(cmd_path, args);
	return (parent_wait(cmd_path, pid));
}
