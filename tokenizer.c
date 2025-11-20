#include "minishell.h"

t_token	*create_token(t_token_type type, char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->value = ft_strdup(value); // ここで値をコピー
	if (!token->value)
	{
		free(token);
		return (NULL);
	}
	token->next = NULL;
	return (token);
}

void	add_token(t_token **head, t_token *new_token)
{
	t_token	*current;

	if (!*head)
	{
		*head = new_token;
		return ;
	}
	current = *head;
	while (current->next)
		current = current->next;
	current->next = new_token;
}

void	free_tokens(t_token *tokens)
{
	t_token	*tmp;

	while (tokens)
	{
		tmp = tokens;
		tokens = tokens->next;
		free(tmp->value);
		free(tmp);
	}
}

void	print_tokens(t_token *tokens)
{
	const char	*type_names[] = {"WORD", "PIPE", "REDIR_IN", "REDIR_OUT",
		"REDIR_APPEND", "REDIR_HEREDOC"};

	printf("\n=== TOKENS ===\n");
	while (tokens)
	{
		printf("Type: %-12s Value: [%s]\n", type_names[tokens->type],
			tokens->value);
		tokens = tokens->next;
	}
	printf("==============\n\n");
}

int	is_special(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

void	skip_space(char **str)
{
	while (**str && (**str == ' ' || **str == '\t'))
		(*str)++;
}

// 構文エラー処理
int	handle_syntax_error(char *value)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd(value, 2);
	ft_putstr_fd("'\n", 2);
	return (0);
}

static char	*strip_quotes(char *raw)
{
	int		i;
	int		j;
	char	quote_char;
	char	*stripped;

	if (!raw)
		return (NULL);
	stripped = malloc(sizeof(char) * (ft_strlen(raw) + 1));
	if (!stripped)
		return (NULL);
	i = 0;
	j = 0;
	quote_char = 0;
	while (raw[i])
	{
		// クォートの開始または終了を判定
		if ((raw[i] == '\'' || raw[i] == '"') && quote_char == 0)
			quote_char = raw[i]; // クォート開始（コピーしない）
		else if (raw[i] == quote_char)
			quote_char = 0; // クォート終了（コピーしない）
		else
			stripped[j++] = raw[i]; // 通常の文字（またはクォート内の別のクォート）はコピー
		i++;
	}
	stripped[j] = '\0';
	return (stripped);
}

// 単語の範囲を切り出す（クォートを含んだままの生の文字列を取得）
char	*get_word(char **str)
{
	char	*start;
	int		len;
	char	quote_char;

	start = *str;
	len = 0;
	while ((*str)[len] && !is_special((*str)[len]) && (*str)[len] != ' ' \
		&& (*str)[len] != '\t')
	{
		if ((*str)[len] == '\'' || (*str)[len] == '"')
		{
			quote_char = (*str)[len];
			len++;
			while ((*str)[len] && (*str)[len] != quote_char)
				len++;
			if (!(*str)[len]) // 閉じクォートがない
			{
				handle_syntax_error("newline");
				return (NULL);
			}
			len++;
		}
		else
			len++;
	}
	*str += len;
	return (ft_substr(start, 0, len));
}

t_token	*tokenize_special(char **str)
{
	t_token_type	type;
	char			*value;

	if (**str == '|')
	{
		type = TOKEN_PIPE;
		value = ft_strdup("|");
		(*str)++;
	}
	else if (**str == '<')
	{
		if (*(*str + 1) == '<')
		{
			type = TOKEN_REDIR_HEREDOC;
			value = ft_strdup("<<");
			(*str) += 2;
		}
		else
		{
			type = TOKEN_REDIR_IN;
			value = ft_strdup("<");
			(*str)++;
		}
	}
	else if (**str == '>')
	{
		if (*(*str + 1) == '>')
		{
			type = TOKEN_REDIR_APPEND;
			value = ft_strdup(">>");
			(*str) += 2;
		}
		else
		{
			type = TOKEN_REDIR_OUT;
			value = ft_strdup(">");
			(*str)++;
		}
	}
	else
		return (NULL);
	return (create_token(type, value));
}

t_token	*tokenize(char *input)
{
	t_token	*tokens;
	t_token	*new_token;
	char	*raw_word;
	char	*clean_word;

	tokens = NULL;
	while (*input)
	{
		skip_space(&input);
		if (!*input)
			break ;
		if (is_special(*input))
		{
			new_token = tokenize_special(&input);
			if (!new_token)
				return (free_tokens(tokens), NULL);
			add_token(&tokens, new_token);
		}
		else
		{
			raw_word = get_word(&input); // まず生の文字列（クォート付き）を取得
			if (!raw_word)
				return (free_tokens(tokens), NULL);
			clean_word = strip_quotes(raw_word);
			free(raw_word); // 生の文字列はもう不要
			
			if (!clean_word) // mallocエラーなど
				return (free_tokens(tokens), NULL);
			new_token = create_token(TOKEN_WORD, clean_word);
			free(clean_word); // create_token内でstrdupされるので解放

			if (!new_token)
				return (free_tokens(tokens), NULL);
			add_token(&tokens, new_token);
		}
	}
	return (tokens);
}
