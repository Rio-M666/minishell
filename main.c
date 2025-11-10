#include "minishell.h"

static void	process_input(char *input, t_shell *shell)
{
	char	**args;

	(void)shell;
	if (!input || input[0] == '\0')
		return ;
	add_history(input);
	args = ft_split(input, ' ');
	if (!args || !args[0])
		return ;
	execute_with_args(args);
	free_array(args);
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