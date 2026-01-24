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

static void	setup_child_pipes(t_pipe_ctx *ctx, int is_last)
{
	if (ctx->prev_fd[0] != -1)
	{
		dup2(ctx->prev_fd[0], STDIN_FILENO);
		close(ctx->prev_fd[0]);
		close(ctx->prev_fd[1]);
	}
	if (!is_last)
	{
		close(ctx->pipe_fd[0]);
		dup2(ctx->pipe_fd[1], STDOUT_FILENO);
		close(ctx->pipe_fd[1]);
	}
}

static void	execute_child_cmd(t_cmd *cmd, char *cmd_path)
{
	extern char	**environ;

	execve(cmd_path, cmd->args, environ);
	perror("execve");
	free(cmd_path);
	exit(126);
}

static void	child_process(t_pipe_ctx *ctx, t_shell *shell)
{
	char	*cmd_path;
	int		is_last;

	is_last = (ctx->current->next == NULL);
	setup_signals_child();
	setup_child_pipes(ctx, is_last);
	if (!apply_redirections(ctx->current->redir_list))
		exit(1);
	if (!ctx->current->args || !ctx->current->args[0])
		exit(0);
	if (is_builtin(ctx->current->args[0]))
		exit(exec_builtin(ctx->current, shell));
	cmd_path = get_command_path(ctx->current->args[0]);
	if (!cmd_path)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(ctx->current->args[0], 2);
		ft_putstr_fd(": command not found\n", 2);
		exit(127);
	}
	execute_child_cmd(ctx->current, cmd_path);
}

static void	parent_process(t_pipe_ctx *ctx)
{
	if (ctx->prev_fd[0] != -1)
	{
		close(ctx->prev_fd[0]);
		close(ctx->prev_fd[1]);
	}
	if (ctx->current->next)
	{
		ctx->prev_fd[0] = ctx->pipe_fd[0];
		ctx->prev_fd[1] = ctx->pipe_fd[1];
	}
}

int	execute_pipeline_cmd(t_pipe_ctx *ctx, t_shell *shell)
{
	pid_t	pid;

	if (ctx->current->next)
	{
		if (pipe(ctx->pipe_fd) == -1)
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
		child_process(ctx, shell);
	parent_process(ctx);
	return (0);
}
