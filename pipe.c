/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 17:36:08 by mrio              #+#    #+#             */
/*   Updated: 2025/12/05 18:13:14 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	apply_redirections(t_redirect *redir_list)
{
	t_redirect	*redir;
	int			fd;

	redir = redir_list;
	while (redir)
	{
		if (redir->type == REDIR_IN)
		{
			fd = open(redir->filename, O_RDONLY);
			if (fd == -1)
			{
				perror(redir->filename);
				return (0);
			}
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		else if (redir->type == REDIR_OUT)
		{
			fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd == -1)
			{
				perror(redir->filename);
				return (0);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (redir->type == REDIR_APPEND)
		{
			fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd == -1)
			{
				perror(redir->filename);
				return (0);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (redir->type == REDIR_HEREDOC)
		{
			if (redir->heredoc_fd == -1)
			{
				ft_putstr_fd("minishell: heredoc: invalid file descriptor\n", 2);
				return (0);
			}
			dup2(redir->heredoc_fd, STDIN_FILENO);
		}
		redir = redir->next;
	}
	return (1);
}

static void	execute_child(t_cmd *cmd, int *prev_pipe_fd, int *pipe_fd,
		int is_last)
{
	char		*cmd_path;
	extern char	**environ;

	setup_signals_child();
	if (prev_pipe_fd != NULL)
	{
		dup2(prev_pipe_fd[0], STDIN_FILENO);
		close(prev_pipe_fd[0]);
		close(prev_pipe_fd[1]);
	}
	if (!is_last)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
	}
	if (!apply_redirections(cmd->redir_list))
		exit(1);
	cmd_path = get_command_path(cmd->args[0]);
	if (!cmd_path)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd->args[0], 2);
		ft_putstr_fd(": command not found\n", 2);
		exit(127);
	}
	execve(cmd_path, cmd->args, environ);
	perror("execve");
	free(cmd_path);
	exit(126);
}

static int	count_commands(t_cmd *pipeline)
{
	int		count;
	t_cmd	*current;

	count = 0;
	current = pipeline;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

int	execute_pipeline(t_cmd *pipeline, t_shell *shell)
{
	int		pipe_fd[2];
	int		prev_pipe_fd[2];
	pid_t	pid;
	int		status;
	int		cmd_count;
	int		i;
	t_cmd	*current;

	(void)shell;
	if (!pipeline)
		return (1);
	cmd_count = count_commands(pipeline);
	if (cmd_count == 1 && !pipeline->redir_list)
	{
		if (!pipeline->args || !pipeline->args[0])
			return (1);
		return (execute_with_args(pipeline->args));
	}
	prev_pipe_fd[0] = -1;
	prev_pipe_fd[1] = -1;
	current = pipeline;
	i = 0;
	while (current)
	{
		if (current->next)
		{
			if (pipe(pipe_fd) == -1)
			{
				perror("pipe");
				return (1);
			}
		}
		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			return (1);
		}
		if (pid == 0)
		{
			execute_child(current, (prev_pipe_fd[0] !=
					-1) ? prev_pipe_fd : NULL, pipe_fd, current->next == NULL);
		}
		if (prev_pipe_fd[0] != -1)
		{
			close(prev_pipe_fd[0]);
			close(prev_pipe_fd[1]);
		}
		if (current->next)
		{
			prev_pipe_fd[0] = pipe_fd[0];
			prev_pipe_fd[1] = pipe_fd[1];
		}
		current = current->next;
		i++;
	}
	i = 0;
	while (i < cmd_count)
	{
		wait(&status);
		i++;
	}
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (1);
}
