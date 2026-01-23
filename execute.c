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

static void	print_cmd_not_found(char *cmd)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd, 2);
	ft_putstr_fd(": command not found\n", 2);
}

static void	child_exec(char *cmd_path, char **args)
{
	extern char	**environ;

	setup_signals_child();
	execve(cmd_path, args, environ);
	perror("execve");
	exit(126);
}

static int	parent_wait(char *cmd_path, pid_t pid)
{
	int	status;

	free(cmd_path);
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (1);
}

int	execute_with_args(char **args)
{
	pid_t	pid;
	char	*cmd_path;

	if (!args || !args[0])
		return (1);
	cmd_path = get_command_path(args[0]);
	if (!cmd_path)
	{
		print_cmd_not_found(args[0]);
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
		child_exec(cmd_path, args);
	return (parent_wait(cmd_path, pid));
}
