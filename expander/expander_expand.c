/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_expand.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 17:27:36 by mrio              #+#    #+#             */
/*   Updated: 2025/11/23 06:35:28 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*make_char_str(char c)
{
	char	*temp;

	temp = malloc(2);
	if (!temp)
		return (NULL);
	temp[0] = c;
	temp[1] = '\0';
	return (temp);
}

static char	*process_char(char *str, int *i, t_shell *shell, t_quote_state *st)
{
	char	*temp;

	if (st->quote_type == QUOTE_NONE)
		update_quote_state(str[*i], st);
	if (str[*i] == '$' && str[*i + 1])
		temp = process_variable(str, i, shell, st);
	else
		temp = make_char_str(str[*i]);
	return (temp);
}

static void	init_expand_state(t_quote_state *st, int *i, t_quote_type qt)
{
	st->in_single = 0;
	st->in_double = 0;
	st->quote_type = qt;
	*i = 0;
}

char	*expand_variables(char *str, t_shell *shell, t_quote_type quote_type)
{
	char			*result;
	t_quote_state	state;
	int				i;
	int				res_len;
	char			*temp;

	result = NULL;
	res_len = 0;
	init_expand_state(&state, &i, quote_type);
	while (str[i])
	{
		temp = process_char(str, &i, shell, &state);
		if (!temp)
		{
			free(result);
			return (NULL);
		}
		result = append_str(result, temp, &i, &res_len);
		if (!result)
			return (NULL);
		i++;
	}
	if (!result)
		return (ft_strdup(""));
	return (result);
}
