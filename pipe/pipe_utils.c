/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: toyamagu <toyamagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 17:36:08 by mrio              #+#    #+#             */
/*   Updated: 2026/01/25 00:24:52 by toyamagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	setup_child_pipes(t_pipe_context *pipe_ctx, int is_last)
{
	if (pipe_ctx->prev_fd[0] != -1)
	{
		dup2(pipe_ctx->prev_fd[0], STDIN_FILENO);
		close(pipe_ctx->prev_fd[0]);
		close(pipe_ctx->prev_fd[1]);
	}
	if (!is_last)
	{
		close(pipe_ctx->pipe_fd[0]);
		dup2(pipe_ctx->pipe_fd[1], STDOUT_FILENO);
		close(pipe_ctx->pipe_fd[1]);
	}
}

static void	execute_child_cmd(t_cmd *cmd, char *cmd_path)
{
	extern char	**environ;

	execve(cmd_path, cmd->args, environ);
	perror("execve");
	free(cmd_path);
	exit(EXIT_CMD_NOT_EXEC);
}

static void	child_process(t_pipe_context *pipe_ctx, t_shell *shell)
{
	char	*cmd_path;
	int		is_last;

	is_last = (pipe_ctx->current->next == NULL);
	setup_signals_child();
	setup_child_pipes(pipe_ctx, is_last);
	if (apply_redirections(pipe_ctx->current->redir_list))
		exit(1);
	if (!pipe_ctx->current->args || !pipe_ctx->current->args[0])
		exit(0);
	if (is_builtin(pipe_ctx->current->args[0]))
		exit(exec_builtin(pipe_ctx->current, shell));
	cmd_path = get_command_path(pipe_ctx->current->args[0]);
	if (!cmd_path)
	{
		print_error(pipe_ctx->current->args[0], "command not found");
		exit(EXIT_CMD_NOT_FOUND);
	}
	execute_child_cmd(pipe_ctx->current, cmd_path);
}

static void	parent_process(t_pipe_context *pipe_ctx)
{
	if (pipe_ctx->prev_fd[0] != -1)
	{
		close(pipe_ctx->prev_fd[0]);
		close(pipe_ctx->prev_fd[1]);
	}
	if (pipe_ctx->current->next)
	{
		pipe_ctx->prev_fd[0] = pipe_ctx->pipe_fd[0];
		pipe_ctx->prev_fd[1] = pipe_ctx->pipe_fd[1];
	}
}

int	execute_pipeline_cmd(t_pipe_context *pipe_ctx, t_shell *shell)
{
	pid_t	pid;

	if (pipe_ctx->current->next)
	{
		if (pipe(pipe_ctx->pipe_fd) == -1)
		{
			perror("pipe");
			return (-1);
		}
	}
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	if (pid == 0)
		child_process(pipe_ctx, shell);
	parent_process(pipe_ctx);
	return (0);
}
