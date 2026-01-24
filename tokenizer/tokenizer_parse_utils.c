/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_parse_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 16:20:12 by mrio              #+#    #+#             */
/*   Updated: 2026/01/23 16:23:40 by mrio             ###   ########.fr       */
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

static int	skip_quoted(char *s, int len)
{
	char	quote_char;

	quote_char = s[len];
	len++;
	while (s[len] && s[len] != quote_char)
		len++;
	if (!s[len])
	{
		handle_syntax_error("newline");
		return (-1);
	}
	return (len + 1);
}

char	*get_word(char **str)
{
	char	*s;
	int		len;

	s = *str;
	len = 0;
	while (s[len] && !is_special(s[len]) && !is_space(s[len]))
	{
		if (s[len] == '\'' || s[len] == '"')
		{
			len = skip_quoted(s, len);
			if (len == -1)
				return (NULL);
		}
		else
			len++;
	}
	*str += len;
	return (ft_substr(s, 0, len));
}
