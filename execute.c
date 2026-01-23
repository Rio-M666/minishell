#include "minishell.h"

/* ==================== PATH解決 ==================== */

/* PATH環境変数からコマンドのフルパスを検索 */
static char	*get_cmd_path(char *cmd)
{
	char	**paths;
	char	*path;
	char	*tmp;
	int		i;

	/* /が含まれていれば絶対/相対パスとして扱う */
	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	paths = ft_split(getenv("PATH"), ':');
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		tmp = ft_strjoin(paths[i], "/");
		path = ft_strjoin(tmp, cmd);
		free(tmp);
		if (access(path, X_OK) == 0)
			return (free_array(paths), path);
		free(path);
		i++;
	}
	free_array(paths);
	return (NULL);
}

/* ==================== リダイレクション ==================== */

/* 全リダイレクトを適用 (子プロセス内で呼ぶ) */
static int	apply_redirects(t_redirect *redir)
{
	int	fd;

	while (redir)
	{
		if (redir->type == REDIR_IN)
			fd = open(redir->filename, O_RDONLY);
		else if (redir->type == REDIR_OUT)
			fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		else if (redir->type == REDIR_APPEND)
			fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else /* REDIR_HEREDOC */
		{
			dup2(redir->heredoc_fd, STDIN_FILENO);
			redir = redir->next;
			continue ;
		}
		if (fd == -1)
			return (perror(redir->filename), 0);
		dup2(fd, (redir->type == REDIR_IN) ? STDIN_FILENO : STDOUT_FILENO);
		close(fd);
		redir = redir->next;
	}
	return (1);
}

/* ==================== Heredoc ==================== */

/* heredoc用のSIGINTハンドラを設定 */
static void	setup_heredoc_signal(void)
{
	struct sigaction	sa;

	sa.sa_handler = handle_sigint_heredoc;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
}

/* heredocの内容を一時ファイルに書き込み、そのfdを返す */
static int	read_heredoc(char *delim, int expand, t_shell *shell)
{
	char	*line;
	char	*expanded;
	int		fd;

	fd = open("/tmp/.minishell_heredoc", O_RDWR | O_CREAT | O_TRUNC, 0600);
	if (fd == -1)
		return (perror("heredoc"), -1);
	unlink("/tmp/.minishell_heredoc"); /* 即座にunlinkしてfdだけ残す */
	setup_heredoc_signal();
	g_signal = 0;
	while (1)
	{
		line = readline("> ");
		if (!line || g_signal == SIGINT)
		{
			if (g_signal == SIGINT)
				return (close(fd), setup_signals_interactive(), -1);
			ft_putstr_fd("minishell: warning: heredoc delimited by EOF\n", 2);
			break ;
		}
		if (ft_strcmp(line, delim) == 0)
		{
			free(line);
			break ;
		}
		/* expand_heredocフラグが立っていれば変数展開 */
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
	lseek(fd, 0, SEEK_SET); /* 読み込み用に先頭に戻す */
	return (fd);
}

/* パイプライン内の全heredocを処理 */
int	process_heredocs(t_cmd *pipeline, t_shell *shell)
{
	t_redirect	*redir;

	while (pipeline)
	{
		redir = pipeline->redir_list;
		while (redir)
		{
			if (redir->type == REDIR_HEREDOC)
			{
				redir->heredoc_fd = read_heredoc(redir->filename,
						redir->expand_heredoc, shell);
				if (redir->heredoc_fd == -1)
					return (0);
			}
			redir = redir->next;
		}
		pipeline = pipeline->next;
	}
	return (1);
}

void	cleanup_heredocs(t_cmd *pipeline)
{
	t_redirect	*redir;

	while (pipeline)
	{
		redir = pipeline->redir_list;
		while (redir)
		{
			if (redir->type == REDIR_HEREDOC && redir->heredoc_fd != -1)
				close(redir->heredoc_fd);
			redir = redir->next;
		}
		pipeline = pipeline->next;
	}
}

/* ==================== コマンド実行 ==================== */

/* 子プロセス内でコマンドを実行 (この関数から戻らない) */
static void	exec_child(t_cmd *cmd, int in_fd, int out_fd)
{
	extern char	**environ;
	char		*path;

	setup_signals_child();
	if (in_fd != STDIN_FILENO)
	{
		dup2(in_fd, STDIN_FILENO);
		close(in_fd);
	}
	if (out_fd != STDOUT_FILENO)
	{
		dup2(out_fd, STDOUT_FILENO);
		close(out_fd);
	}
	if (!apply_redirects(cmd->redir_list))
		exit(1);
	if (!cmd->args || !cmd->args[0])
		exit(0);
	path = get_cmd_path(cmd->args[0]);
	if (!path)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd->args[0], 2);
		ft_putstr_fd(": command not found\n", 2);
		exit(127);
	}
	execve(path, cmd->args, environ);
	perror("execve");
	exit(126);
}

/*
 * パイプラインを実行
 * 各コマンドをforkして、パイプで繋ぐ
 */
int	execute_pipeline(t_cmd *pipeline, t_shell *shell)
{
	int		pipefd[2];
	int		prev_fd;
	int		status;
	int		cmd_count;
	pid_t	pid;
	t_cmd	*cmd;

	(void)shell;
	if (!pipeline)
		return (1);
	prev_fd = STDIN_FILENO;
	cmd_count = 0;
	cmd = pipeline;
	while (cmd)
	{
		/* 次のコマンドがあればパイプを作成 */
		if (cmd->next && pipe(pipefd) == -1)
			return (perror("pipe"), 1);
		pid = fork();
		if (pid == -1)
			return (perror("fork"), 1);
		if (pid == 0)
		{
			/* 子: 入力をprev_fd、出力を次のパイプ(あれば)に繋ぐ */
			if (cmd->next)
				close(pipefd[0]);
			exec_child(cmd, prev_fd, cmd->next ? pipefd[1] : STDOUT_FILENO);
		}
		/* 親: 前のパイプを閉じて、次への入力を保存 */
		if (prev_fd != STDIN_FILENO)
			close(prev_fd);
		if (cmd->next)
		{
			close(pipefd[1]);
			prev_fd = pipefd[0];
		}
		cmd_count++;
		cmd = cmd->next;
	}
	/* 全子プロセスを待つ */
	while (cmd_count-- > 0)
		wait(&status);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (1);
}
