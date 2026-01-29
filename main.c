/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 16:52:11 by mrio              #+#    #+#             */
/*   Updated: 2026/01/23 16:52:13 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal = 0;

static void	process_input(char *input, t_shell *shell)
{
	t_token	*tokens;
	t_cmd	*pipeline;

	if (!input || input[0] == '\0')
		return ;
	add_history(input);
	tokens = tokenize(input);
	if (!tokens)
		return ;
	expand_tokens(tokens, shell);
	pipeline = parse(tokens);
	free_tokens(tokens);
	if (!pipeline)
		return ;
	if (process_heredocs(pipeline, shell))
	{
		cleanup_heredocs(pipeline);
		free_pipeline(pipeline);
		return ;
	}
	shell->last_status = execute_pipeline(pipeline, shell);
	cleanup_heredocs(pipeline);
	free_pipeline(pipeline);
}

int	main(int argc, char **argv, char **envp)
{
	char	*line;
	t_shell	shell;

	(void)argc;
	(void)argv;
	shell.envp = init_envp(envp);
	shell.last_status = 0;
	setup_signals_interactive();
	while (1)
	{
		line = readline("minishell> ");
		if (!line)
		{
			write(1, "exit\n", 5);
			break ;
		}
		process_input(line, &shell);
		free(line);
	}
	return (0);
}
