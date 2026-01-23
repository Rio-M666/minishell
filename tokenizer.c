#include "minishell.h"

void	free_tokens(t_token *tokens)
{
	t_token	*tmp;

	while (tokens)
	{
		tmp = tokens->next;
		free(tokens->value);
		free(tokens);
		tokens = tmp;
	}
}

/* クォート文字を除去した文字列を返す (例: "hello" -> hello) */
static char	*strip_quotes(char *raw)
{
	char	*result;
	char	quote;
	int		i;
	int		j;

	result = malloc(ft_strlen(raw) + 1);
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	quote = 0;
	while (raw[i])
	{
		if ((raw[i] == '\'' || raw[i] == '"') && !quote)
			quote = raw[i];
		else if (raw[i] == quote)
			quote = 0;
		else
			result[j++] = raw[i];
		i++;
	}
	result[j] = '\0';
	return (result);
}

/* 入力文字列から1つの単語(WORDトークン)を切り出す */
static char	*get_word(char **input)
{
	char	*start;
	char	quote;
	int		len;

	start = *input;
	len = 0;
	while (start[len] && !is_space(start[len])
		&& start[len] != '|' && start[len] != '<' && start[len] != '>')
	{
		if (start[len] == '\'' || start[len] == '"')
		{
			quote = start[len++];
			while (start[len] && start[len] != quote)
				len++;
			if (!start[len])
			{
				ft_putstr_fd("minishell: syntax error: unclosed quote\n", 2);
				return (NULL);
			}
		}
		len++;
	}
	*input += len;
	return (ft_substr(start, 0, len));
}

/* トークンをリストの末尾に追加 */
static void	add_token(t_token **list, t_token *new)
{
	t_token	*cur;

	if (!*list)
		*list = new;
	else
	{
		cur = *list;
		while (cur->next)
			cur = cur->next;
		cur->next = new;
	}
}

/* 特殊文字(|, <, >, <<, >>)をトークン化 */
static t_token	*tokenize_operator(char **input)
{
	t_token		*tok;
	char		c;

	c = **input;
	tok = malloc(sizeof(t_token));
	if (!tok)
		return (NULL);
	tok->quote = QUOTE_NONE;
	tok->next = NULL;
	if (c == '|')
	{
		tok->type = TOKEN_PIPE;
		tok->value = ft_strdup("|");
		(*input)++;
	}
	else if (c == '<' || c == '>')
	{
		/* << or >> の場合は2文字進める */
		if ((*input)[1] == c)
		{
			tok->type = (c == '<') ? TOKEN_REDIR_HEREDOC : TOKEN_REDIR_APPEND;
			tok->value = ft_strdup((c == '<') ? "<<" : ">>");
			*input += 2;
		}
		else
		{
			tok->type = (c == '<') ? TOKEN_REDIR_IN : TOKEN_REDIR_OUT;
			tok->value = ft_strdup((c == '<') ? "<" : ">");
			(*input)++;
		}
	}
	return (tok);
}

t_token	*tokenize(char *input)
{
	t_token	*tokens;
	t_token	*tok;
	char	*raw;
	char	*clean;

	tokens = NULL;
	while (*input)
	{
		while (*input && is_space(*input))
			input++;
		if (!*input)
			break ;
		/* 演算子(|<>)かWORDかで処理を分岐 */
		if (*input == '|' || *input == '<' || *input == '>')
		{
			tok = tokenize_operator(&input);
			if (!tok)
				return (free_tokens(tokens), NULL);
		}
		else
		{
			raw = get_word(&input);
			if (!raw)
				return (free_tokens(tokens), NULL);
			clean = strip_quotes(raw);
			tok = malloc(sizeof(t_token));
			tok->type = TOKEN_WORD;
			tok->value = clean;
			/* 最初の文字がクォートならその種類を記録(展開判定用) */
			tok->quote = (raw[0] == '\'') ? QUOTE_SINGLE :
						(raw[0] == '"') ? QUOTE_DOUBLE : QUOTE_NONE;
			tok->next = NULL;
			free(raw);
		}
		add_token(&tokens, tok);
	}
	return (tokens);
}
