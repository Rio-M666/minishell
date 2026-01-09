/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 13:15:40 by mrio              #+#    #+#             */
/*   Updated: 2026/01/09 13:15:41 by mrio             ###   ########.fr       */
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
	// 1. Tokenize
	tokens = tokenize(input);
	if (!tokens)
		return ;
	// print_tokens(tokens); // デバッグ用
	// 2. Expand variables
	expand_tokens(tokens, shell);
	// 3. Parse
	pipeline = parse(tokens);
	free_tokens(tokens); // Parseが終わればTokenは不要
	if (!pipeline)
		return ;
	// print_pipeline(pipeline); // デバッグ用
	// 4. Process heredocs
	if (!process_heredocs(pipeline, shell))
	{
		cleanup_heredocs(pipeline);
		free_pipeline(pipeline);
		return ;
	}
	// 5. Execute
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
	shell.envp = envp; // 環境変数を保持
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
