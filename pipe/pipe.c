/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 17:36:08 by mrio              #+#    #+#             */
/*   Updated: 2026/01/23 15:41:23 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

static void	wait_for_children(int cmd_count, int *status)
{
	int	i;

	i = 0;
	while (i < cmd_count)
	{
		wait(status);
		i++;
	}
}

static int	handle_single_command(t_cmd *pipeline)
{
	if (!pipeline->args || !pipeline->args[0])
		return (1);
	return (execute_with_args(pipeline->args));
}

int	execute_pipeline(t_cmd *pipeline, t_shell *shell)
{
	t_pipe_ctx	ctx;
	int			status;

	(void)shell;
	if (!pipeline)
		return (1);
	ctx.cmd_count = count_commands(pipeline);
	if (ctx.cmd_count == 1 && !pipeline->redir_list)
		return (handle_single_command(pipeline));
	ctx.prev_fd[0] = -1;
	ctx.prev_fd[1] = -1;
	ctx.current = pipeline;
	while (ctx.current)
	{
		if (execute_pipeline_cmd(&ctx) == -1)
			return (1);
		ctx.current = ctx.current->next;
	}
	wait_for_children(ctx.cmd_count, &status);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (1);
}
