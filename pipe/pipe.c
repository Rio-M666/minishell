/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: toyamagu <toyamagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 17:36:08 by mrio              #+#    #+#             */
/*   Updated: 2026/01/25 00:21:29 by toyamagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	exec_single_builtin(t_cmd *cmd, t_shell *shell)
{
	int	saved_fd[2];
	int	status;

	saved_fd[0] = dup(STDIN_FILENO);
	saved_fd[1] = dup(STDOUT_FILENO);
	if (apply_redirections(cmd->redir_list))
	{
		status = exec_builtin(cmd, shell);
		dup2(saved_fd[0], STDIN_FILENO);
		dup2(saved_fd[1], STDOUT_FILENO);
		close(saved_fd[0]);
		close(saved_fd[1]);
		return (status);
	}
	dup2(saved_fd[0], STDIN_FILENO);
	dup2(saved_fd[1], STDOUT_FILENO);
	close(saved_fd[0]);
	close(saved_fd[1]);
	return (1);
}

static int	get_last_status(int status)
{
	if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGINT || WTERMSIG(status) == SIGQUIT)
			write(1, "\n", 1);
		return (128 + WTERMSIG(status));
	}
	else if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (0);
}

static int	wait_pipeline(int cmd_count)
{
	int	i;
	int	status;
	int	last_status;

	i = 0;
	last_status = 0;
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	while (i < cmd_count)
	{
		wait(&status);
		if (i == cmd_count - 1)
			last_status = get_last_status(status);
		i++;
	}
	setup_signals_interactive();
	return (last_status);
}

static int	count_cmds(t_cmd *pipeline)
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
	t_pipe_ctx	ctx;

	if (!pipeline)
		return (0);
	ctx.cmd_count = count_cmds(pipeline);
	if (ctx.cmd_count == 1 && pipeline->args && is_builtin(pipeline->args[0]))
		return (exec_single_builtin(pipeline, shell));
	if (ctx.cmd_count == 1 && !pipeline->redir_list)
		return (execute_with_args(pipeline->args));
	ctx.prev_fd[0] = -1;
	ctx.prev_fd[1] = -1;
	ctx.current = pipeline;
	while (ctx.current)
	{
		if (execute_pipeline_cmd(&ctx, shell) == -1)
			return (1);
		ctx.current = ctx.current->next;
	}
	return (wait_pipeline(ctx.cmd_count));
}
