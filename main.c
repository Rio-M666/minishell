#include "minishell.h"

volatile sig_atomic_t	g_signal = 0;

/* ========== SIGNAL HANDLERS ========== */

static void	handle_sigint_interactive(int sig)
{
	(void)sig;
	g_signal = SIGINT;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_redisplay();
}

void	handle_sigint_heredoc(int sig)
{
	(void)sig;
	g_signal = SIGINT;
	write(1, "\n", 1);
	close(STDIN_FILENO);
}

void	setup_signals_interactive(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	sa_int.sa_handler = handle_sigint_interactive;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);
	sa_quit.sa_handler = SIG_IGN;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	sigaction(SIGQUIT, &sa_quit, NULL);
}

void	setup_signals_child(void)
{
	struct sigaction	sa;

	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}

/* ========== MAIN LOOP ========== */

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
	if (!process_heredocs(pipeline, shell))
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
	shell.envp = envp;
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
