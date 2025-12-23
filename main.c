#include "minishell.h"

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

	// 3. Execute
	shell->last_status = execute_pipeline(pipeline, shell);

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
