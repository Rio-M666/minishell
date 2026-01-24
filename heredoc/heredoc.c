/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 16:00:00 by mrio              #+#    #+#             */
/*   Updated: 2026/01/23 18:55:35 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	process_single_heredoc(t_redirect *redir, t_shell *shell)
{
	int	fd;

	if (redir->type != REDIR_HEREDOC)
		return (1);
	fd = read_heredoc_content(redir->filename, redir->expand_heredoc, shell);
	if (fd == -1)
		return (0);
	redir->heredoc_fd = fd;
	return (1);
}

int	process_heredocs(t_cmd *pipeline, t_shell *shell)
{
	t_cmd		*cmd;
	t_redirect	*redir;

	cmd = pipeline;
	while (cmd)
	{
		redir = cmd->redir_list;
		while (redir)
		{
			if (redir->type == REDIR_HEREDOC)
			{
				if (!process_single_heredoc(redir, shell))
					return (0);
			}
			redir = redir->next;
		}
		cmd = cmd->next;
	}
	return (1);
}

void	cleanup_heredocs(t_cmd *pipeline)
{
	t_cmd		*cmd;
	t_redirect	*redir;

	cmd = pipeline;
	while (cmd)
	{
		redir = cmd->redir_list;
		while (redir)
		{
			if (redir->type == REDIR_HEREDOC && redir->heredoc_fd != -1)
			{
				close(redir->heredoc_fd);
				redir->heredoc_fd = -1;
			}
			redir = redir->next;
		}
		cmd = cmd->next;
	}
}
