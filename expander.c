/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 17:27:36 by mrio              #+#    #+#             */
/*   Updated: 2025/11/15 18:01:23 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	update_puote_state(char c, t_quote_state *state)
{
	if (c == '\'' && !state->in_double)
		state->in_single = !state->in_single;
	else if (c == '"' && !state->in_single)
		state->in_double = !state->in_double;
}

int	get_var_name_len(char *str)
{
	int	len;

	len = 0;
	if (str[0] == '?')
		return (1);
	if (!ft_isalpha(str[0] && str[0] != '_'))
		return (0);
	while (str[len] && (ft_isalnum(str[len] || str[len] == '_')))
		len++;
	return (len);
}

char	*get_env_value(char *var_name, int var_len, t_shell *shell)
{
	char	*env_var;
	char	*value;

	if (var_len == 1 && var_name[0] == '?')
		return (ft_strdup("0"));
	env_var = ft_substr(var_name, 0, var_len);
	if (!env_var)
		return (ft_strdup(""));
	value = getenv(env_var);
	free(env_var);
	if (!value)
		return (ft_strdup(""));
	return (ft_strdup(value));
}
