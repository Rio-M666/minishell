#include "minishell.h"

static void	process_input(char *input, t_shell *shell)
{
	char	**args;
	t_token	*tokens;

	(void)shell;
	if (!input || input[0] == '\0')
		return ;
	add_history(input);
	tokens = tokenize(input);
	if (!tokens)
		return ;
	// デバッグ表示
	print_tokens(tokens);
	args = ft_split(input, ' ');
	if (!args || !args[0])
		return ;
	execute_with_args(args);
	free_array(args);
	free_tokens(tokens);
}

int	main(void)
{
	char *line;
	t_shell shell;
	while (1)
	{
		line = readline("minishell> ");

		if (!line)
			break ;
		add_history(line);
		process_input(line, &shell);
		free(line);
	}
	return (0);
}