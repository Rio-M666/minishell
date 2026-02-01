/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 16:00:00 by mrio              #+#    #+#             */
/*   Updated: 2026/01/23 18:56:14 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	setup_signals_heredoc(void)
{
	struct sigaction	sa_int;

	sa_int.sa_handler = handle_sigint_heredoc;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);
}

static void	print_heredoc_warning(char *marker)
{
	ft_putstr_fd("minishell: warning: ", 2);
	ft_putstr_fd("here-document delimited by end-of-file (wanted `", 2);
	ft_putstr_fd(marker, 2);
	ft_putstr_fd("')\n", 2);
}

static void	write_heredoc_line(int fd, char *line, int expand, t_shell *shell)
{
	char	*expanded;

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

static int	read_heredoc_loop(int fd, char *marker, int expand, t_shell *shell)
{
	char	*line;

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
			print_heredoc_warning(marker);
			break ;
		}
		if (ft_strcmp(line, marker) == 0)
		{
			free(line);
			break ;
		}
		write_heredoc_line(fd, line, expand, shell);
	}
	return (0);
}

int	read_heredoc_content(char *marker, int expand, t_shell *shell)
{
	char	*template;
	int		fd;
	int		ret;

	template = ft_strdup("/tmp/.heredoc_XXXXXX");
	if (!template)
		return (-1);
	fd = mkstemp(template);
	free(template);
	if (fd == -1)
		return (perror("mkstemp"), -1);
	unlink(template);
	setup_signals_heredoc();
	g_signal = 0;
	ret = read_heredoc_loop(fd, marker, expand, shell);
	if (ret == -1)
		return (-1);
	setup_signals_interactive();
	lseek(fd, 0, SEEK_SET);
	return (fd);
}
