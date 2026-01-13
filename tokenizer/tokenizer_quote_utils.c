/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_quote_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 13:16:11 by mrio              #+#    #+#             */
/*   Updated: 2026/01/09 13:16:12 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_quote_type	get_quote_type(char *raw)
{
	if (!raw || !raw[0])
		return (QUOTE_NONE);
	if (raw[0] == '\'')
		return (QUOTE_SINGLE);
	if (raw[0] == '"')
		return (QUOTE_DOUBLE);
	return (QUOTE_NONE);
}

char	*strip_quotes(char *raw)
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
		if ((raw[i] == '\'' || raw[i] == '"') && quote_char == 0)
			quote_char = raw[i];
		else if (raw[i] == quote_char)
			quote_char = 0;
		else
			stripped[j++] = raw[i];
		i++;
	}
	stripped[j] = '\0';
	return (stripped);
}
