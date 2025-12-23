/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 00:00:00 by mrio              #+#    #+#             */
/*   Updated: 2025/12/23 00:00:00 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	create_heredoc_tempfile(char **path_out)
{
	char	template[] = "/tmp/.minishell_heredoc_XXXXXX";
	int		fd;
	char	*path;

	fd = mkstemp(template);
	if (fd == -1)
		return (-1);
	path = ft_strdup(template);
	if (!path)
	{
		close(fd);
		unlink(template);
		return (-1);
	}
	*path_out = path;
	return (fd);
}

static int	delimiter_matches(char *line, char *delimiter)
{
	int	i;

	i = 0;
	while (line[i] && delimiter[i] && line[i] == delimiter[i])
		i++;
	if (line[i] == '\0' && delimiter[i] == '\0')
		return (1);
	return (0);
}

static int	read_heredoc_content(int fd, char *delimiter, int expand,
		t_shell *shell)
{
	char	*line;
	char	*expanded;

	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			ft_putstr_fd("minishell: warning: here-document delimited by ", 2);
			ft_putstr_fd("end-of-file (wanted `", 2);
			ft_putstr_fd(delimiter, 2);
			ft_putstr_fd("')\n", 2);
			return (0);
		}
		if (delimiter_matches(line, delimiter))
		{
			free(line);
			return (1);
		}
		if (expand)
		{
			expanded = expand_variables(line, shell);
			free(line);
			if (!expanded)
				return (0);
			line = expanded;
		}
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
		free(line);
	}
	return (1);
}

static int	process_single_heredoc(t_redirect *redir, t_shell *shell)
{
	char	*temp_path;
	int		fd;
	int		expand;
	int		result;

	temp_path = NULL;
	fd = create_heredoc_tempfile(&temp_path);
	if (fd == -1)
	{
		perror("mkstemp");
		return (0);
	}
	expand = 1;
	result = read_heredoc_content(fd, redir->filename, expand, shell);
	close(fd);
	if (!result)
	{
		unlink(temp_path);
		free(temp_path);
		return (0);
	}
	free(redir->filename);
	redir->filename = temp_path;
	return (1);
}

int	process_heredocs(t_cmd *pipeline, t_shell *shell)
{
	t_cmd		*current_cmd;
	t_redirect	*current_redir;

	current_cmd = pipeline;
	while (current_cmd)
	{
		current_redir = current_cmd->redir_list;
		while (current_redir)
		{
			if (current_redir->type == REDIR_HEREDOC)
			{
				if (!process_single_heredoc(current_redir, shell))
				{
					cleanup_heredocs(pipeline);
					return (0);
				}
			}
			current_redir = current_redir->next;
		}
		current_cmd = current_cmd->next;
	}
	return (1);
}

void	cleanup_heredocs(t_cmd *pipeline)
{
	t_cmd		*current_cmd;
	t_redirect	*current_redir;

	current_cmd = pipeline;
	while (current_cmd)
	{
		current_redir = current_cmd->redir_list;
		while (current_redir)
		{
			if (current_redir->type == REDIR_HEREDOC
				&& current_redir->filename
				&& ft_strlen(current_redir->filename) > 0
				&& current_redir->filename[0] == '/')
			{
				unlink(current_redir->filename);
			}
			current_redir = current_redir->next;
		}
		current_cmd = current_cmd->next;
	}
}
