/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 17:36:08 by mrio              #+#    #+#             */
/*   Updated: 2026/01/23 15:41:23 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	apply_redir_in(t_redirect *redir)
{
	int	fd;

	fd = open(redir->filename, O_RDONLY);
	if (fd == -1)
	{
		perror(redir->filename);
		return (1);
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
	return (0);
}

static int	apply_redir_out(t_redirect *redir)
{
	int	fd;

	fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, DEFAULT_FILE_PERMS);
	if (fd == -1)
	{
		perror(redir->filename);
		return (1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

static int	apply_redir_append(t_redirect *redir)
{
	int	fd;

	fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, DEFAULT_FILE_PERMS);
	if (fd == -1)
	{
		perror(redir->filename);
		return (1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

static int	apply_redir_heredoc(t_redirect *redir)
{
	if (redir->heredoc_fd == -1)
	{
		ft_putstr_fd("minishell: heredoc: invalid file descriptor\n", 2);
		return (1);
	}
	dup2(redir->heredoc_fd, STDIN_FILENO);
	return (0);
}

int	apply_redirections(t_redirect *redir_list)
{
	t_redirect	*redir;

	redir = redir_list;
	while (redir)
	{
		if (redir->type == REDIR_IN && apply_redir_in(redir))
			return (1);
		else if (redir->type == REDIR_OUT && apply_redir_out(redir))
			return (1);
		else if (redir->type == REDIR_APPEND && apply_redir_append(redir))
			return (1);
		else if (redir->type == REDIR_HEREDOC && apply_redir_heredoc(redir))
			return (1);
		redir = redir->next;
	}
	return (0);
}
