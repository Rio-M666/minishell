/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 17:27:36 by mrio              #+#    #+#             */
/*   Updated: 2025/11/23 06:35:28 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	update_quote_state(char c, t_quote_state *state)
{
	if (c == '\'' && !state->in_double)
		state->in_single = !state->in_single;
	else if (c == '"' && !state->in_single)
		state->in_double = !state->in_double;
}

char	*append_str(char *result, char *str, int *i, int *res_len)
{
	char	*new_result;
	int		j;
	int		k;

	(void)i;
	if (!result)
		new_result = ft_strdup(str);
	else
	{
		new_result = malloc(*res_len + ft_strlen(str) + 1);
		if (!new_result)
		{
			free(result);
			free(str);
			return (NULL);
		}
		j = 0;
		while (result[j])
		{
			new_result[j] = result[j];
			j++;
		}
		k = 0;
		while (str[k])
			new_result[j++] = str[k++];
		new_result[j] = '\0';
		free(result);
	}
	*res_len = ft_strlen(new_result);
	free(str);
	return (new_result);
}
