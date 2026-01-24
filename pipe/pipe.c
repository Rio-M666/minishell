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

int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (!ft_strcmp(cmd, "echo") || !ft_strcmp(cmd, "cd") || !ft_strcmp(cmd,
			"pwd") || !ft_strcmp(cmd, "export") || !ft_strcmp(cmd, "unset")
		|| !ft_strcmp(cmd, "env") || !ft_strcmp(cmd, "exit"))
		return (1);
	return (0);
}

int	exec_builtin(t_cmd *cmd, t_shell *shell)
{
	if (!ft_strcmp(cmd->args[0], "echo"))
		return (ft_echo(cmd->args));
	if (!ft_strcmp(cmd->args[0], "cd"))
		return (ft_cd(cmd->args, shell));
	if (!ft_strcmp(cmd->args[0], "pwd"))
		return (ft_pwd());
	if (!ft_strcmp(cmd->args[0], "export"))
		return (ft_export(cmd->args, shell));
	if (!ft_strcmp(cmd->args[0], "unset"))
		return (ft_unset(cmd->args, shell));
	if (!ft_strcmp(cmd->args[0], "env"))
		return (ft_env(shell));
	if (!ft_strcmp(cmd->args[0], "exit"))
		return (ft_exit(cmd->args, shell));
	return (0);
}

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

static int	wait_pipeline(int cmd_count)
{
	int	i;
	int	status;
	int	last_status;

	i = 0;
	last_status = 0;
	while (i < cmd_count)
	{
		wait(&status);
		if (WIFEXITED(status))
			last_status = WEXITSTATUS(status);
		i++;
	}
	return (last_status);
}

int	execute_pipeline(t_cmd *pipeline, t_shell *shell)
{
	t_pipe_ctx ctx;

	if (!pipeline)
		return (0);
	ctx.cmd_count = 0;
	ctx.current = pipeline;
	while (ctx.current)
	{
		ctx.cmd_count++;
		ctx.current = ctx.current->next;
	}
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
