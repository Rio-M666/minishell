/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 21:55:12 by mrio              #+#    #+#             */
/*   Updated: 2025/11/15 16:12:36 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_special(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

void	skip_space(char **str)
{
	while (**str && (**str == ' ' || **str == '\t'))
		(*str)++;
}

t_token	*create_token(t_token_type type, char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->value = ft_strdup(value);
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

char	*get_word(char **str, int *error)
{
	char	*start;
	int		len;
	char	quote;

	start = *str;
	len = 0;
	*error = 0;
	while ((*str)[len] && !is_special((*str)[len]) && (*str)[len] != ' '
		&& (*str)[len] != '\t')
	{
		if ((*str)[len] == '"' || (*str)[len] == '\'')
		{
			quote = (*str)[len];
			len++;
			while ((*str)[len] && (*str)[len] != quote)
				len++;
			if ((*str)[len] == '\0')
			{
				*error = 1;
				write(STDERR_FILENO, "minishell: unclosed quote\n", 26);
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
	char	*word;
	int		error;

	tokens = NULL;
	while (*input)
	{
		skip_space(&input);
		if (!*input)
			break ;
		if (is_special(*input))
		{
			new_token = tokenize_special(&input);
			add_token(&tokens, new_token);
		}
		else
		{
			word = get_word(&input, &error);
			if (error)
			{
				free_tokens(tokens);
				return (NULL);
			}
			if (word && *word)
			{
				new_token = create_token(TOKEN_WORD, word);
				add_token(&tokens, new_token);
			}
			free(word);
		}
	}
	return (tokens);
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

//デバッグ用
void	print_tokens(t_token *tokens)
{
	const char *type_names[] = {"WORD", "PIPE", "REDIR_IN", "REDIR_OUT",
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