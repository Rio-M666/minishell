#include "minishell.h"

/* シンタックスエラーを出力 */
static void	syntax_error(char *token)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd(token ? token : "newline", 2);
	ft_putstr_fd("'\n", 2);
}

void	free_pipeline(t_cmd *pipeline)
{
	t_cmd		*cmd_tmp;
	t_redirect	*redir;
	t_redirect	*redir_tmp;

	while (pipeline)
	{
		cmd_tmp = pipeline->next;
		free_array(pipeline->args);
		redir = pipeline->redir_list;
		while (redir)
		{
			redir_tmp = redir->next;
			free(redir->filename);
			free(redir);
			redir = redir_tmp;
		}
		free(pipeline);
		pipeline = cmd_tmp;
	}
}

/* パイプまでのWORDトークン数をカウント (args配列のサイズ決定用) */
static int	count_args(t_token *tok)
{
	int	count;

	count = 0;
	while (tok && tok->type != TOKEN_PIPE)
	{
		if (tok->type == TOKEN_WORD)
			count++;
		else
			tok = tok->next;
		if (tok)
			tok = tok->next;
	}
	return (count);
}

/* リダイレクトトークンを処理してcmdに追加 */
static int	add_redirect(t_cmd *cmd, t_token **tok)
{
	t_redirect	*redir;
	t_redirect	*last;
	t_redir_type	types[] = {REDIR_IN, REDIR_OUT, REDIR_APPEND, REDIR_HEREDOC};
	t_token_type	type;

	type = (*tok)->type;
	*tok = (*tok)->next;
	if (!*tok || (*tok)->type != TOKEN_WORD)
		return (syntax_error(*tok ? (*tok)->value : NULL), 0);
	redir = malloc(sizeof(t_redirect));
	if (!redir)
		return (0);
	redir->type = types[type - TOKEN_REDIR_IN];
	redir->filename = ft_strdup((*tok)->value);
	redir->heredoc_fd = -1;
	redir->expand_heredoc = (type == TOKEN_REDIR_HEREDOC
			&& (*tok)->quote == QUOTE_NONE);
	redir->next = NULL;
	if (!cmd->redir_list)
		cmd->redir_list = redir;
	else
	{
		last = cmd->redir_list;
		while (last->next)
			last = last->next;
		last->next = redir;
	}
	*tok = (*tok)->next;
	return (1);
}

/* パイプまでの1コマンドをパース */
static t_cmd	*parse_command(t_token **tok)
{
	t_cmd	*cmd;
	int		arg_count;
	int		i;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->redir_list = NULL;
	cmd->next = NULL;
	arg_count = count_args(*tok);
	cmd->args = malloc(sizeof(char *) * (arg_count + 1));
	if (!cmd->args)
		return (free(cmd), NULL);
	i = 0;
	while (*tok && (*tok)->type != TOKEN_PIPE)
	{
		if ((*tok)->type == TOKEN_WORD)
			cmd->args[i++] = ft_strdup((*tok)->value);
		else if (!add_redirect(cmd, tok))
			return (free_pipeline(cmd), NULL);
		if (*tok && (*tok)->type == TOKEN_WORD)
			*tok = (*tok)->next;
	}
	cmd->args[i] = NULL;
	return (cmd);
}

/* トークン列をパースしてコマンドのリンクドリスト(パイプライン)を構築 */
t_cmd	*parse(t_token *tokens)
{
	t_cmd	*head;
	t_cmd	*tail;
	t_cmd	*cmd;

	if (!tokens || tokens->type == TOKEN_PIPE)
		return (syntax_error(tokens ? "|" : NULL), NULL);
	head = NULL;
	tail = NULL;
	while (tokens)
	{
		cmd = parse_command(&tokens);
		if (!cmd)
			return (free_pipeline(head), NULL);
		if (!head)
			head = cmd;
		else
			tail->next = cmd;
		tail = cmd;
		if (tokens && tokens->type == TOKEN_PIPE)
		{
			tokens = tokens->next;
			if (!tokens || tokens->type == TOKEN_PIPE)
				return (syntax_error(tokens ? "|" : NULL), free_pipeline(head), NULL);
		}
	}
	return (head);
}
