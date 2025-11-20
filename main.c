#include "minishell.h"

static void	process_input(char *input, t_shell *shell)
{
	t_token	*tokens;
	t_cmd	*pipeline;
	t_cmd	*current_cmd;

	if (!input || input[0] == '\0')
		return ;
	add_history(input);
	
	// 1. Tokenize
	tokens = tokenize(input);
	if (!tokens)
		return ;
	// print_tokens(tokens); // デバッグ用

	// 2. Parse
	pipeline = parse(tokens);
	free_tokens(tokens); // Parseが終わればTokenは不要
	if (!pipeline)
		return ;
	// print_pipeline(pipeline); // デバッグ用

	// 3. Execute
	// (注: 今回の execute.c はパイプを処理しない単発実行用ですが、ループで回します)
	current_cmd = pipeline;
	while (current_cmd)
	{
		if (current_cmd->args && current_cmd->args[0])
		{
			// パイプラインのサポートは execute.c に実装されていないため、
			// ここでは単純に順番に実行します。
			shell->last_status = execute_with_args(current_cmd->args);
		}
		current_cmd = current_cmd->next;
	}

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
