/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 16:00:00 by mrio              #+#    #+#             */
/*   Updated: 2026/01/06 16:00:00 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	setup_signals_heredoc(void)
{
	struct sigaction	sa_int;

	sa_int.sa_handler = handle_sigint_heredoc;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);
}

static int	read_heredoc_content(char *delimiter, int expand, t_shell *shell)
{
	char	template[] = "/tmp/.heredoc_XXXXXX";
	int		fd;
	char	*line;
	char	*expanded;

	fd = mkstemp(template);
	if (fd == -1)
		return (perror("mkstemp"), -1);
	unlink(template);
	setup_signals_heredoc();
	g_signal = 0;
	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			if (g_signal == SIGINT)
			{
				close(fd);
				setup_signals_interactive();
				return (-1);
			}
			ft_putstr_fd("minishell: warning: here-document delimited by end-of-file (wanted `", 2);
			ft_putstr_fd(delimiter, 2);
			ft_putstr_fd("')\n", 2);
			break ;
		}
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		if (expand)
		{
			expanded = expand_variables(line, shell);
			write(fd, expanded, ft_strlen(expanded));
			free(expanded);
		}
		else
			write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
		free(line);
	}
	setup_signals_interactive();
	lseek(fd, 0, SEEK_SET);
	return (fd);
}

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
