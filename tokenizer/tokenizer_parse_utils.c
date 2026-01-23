/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_parse_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 13:16:17 by mrio              #+#    #+#             */
/*   Updated: 2026/01/09 13:16:18 by mrio             ###   ########.fr       */
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

char	*get_word(char **str)
{
	char	*s;
	int		len;
	char	quote_char;

	s = *str;
	len = 0;
	while (s[len] && !is_special(s[len]) && !is_space(s[len]))
	{
		if (s[len] == '\'' || s[len] == '"')
		{
			quote_char = s[len];
			len++;
			while (s[len] && s[len] != quote_char)
				len++;
			if (!s[len])
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
	return (ft_substr(s, 0, len));
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
