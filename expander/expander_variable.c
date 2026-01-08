/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_variable.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 17:27:36 by mrio              #+#    #+#             */
/*   Updated: 2025/11/23 06:35:28 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	get_var_name_len(char *str)
{
	int	len;

	len = 0;
	if (str[0] == '?')
		return (1);
	if (!ft_isalpha(str[0]) && str[0] != '_')
		return (0);
	while (str[len] && (ft_isalnum(str[len]) || str[len] == '_'))
		len++;
	return (len);
}

char	*get_env_value(char *var_name, int var_len, t_shell *shell)
{
	char	*env_var;
	char	*value;
	char	*status_str;
	int		status;

	if (var_len == 1 && var_name[0] == '?')
	{
		status = shell->last_status;
		status_str = malloc(12);
		if (!status_str)
			return (ft_strdup(""));
		snprintf(status_str, 12, "%d", status);
		return (status_str);
	}
	env_var = ft_substr(var_name, 0, var_len);
	if (!env_var)
		return (ft_strdup(""));
	value = getenv(env_var);
	free(env_var);
	if (!value)
		return (ft_strdup(""));
	return (ft_strdup(value));
}

char	*process_variable(char *str, int *i, t_shell *shell,
		t_quote_state *state)
{
	int		var_len;
	char	*value;

	(*i)++;
	if (state->in_single)
	{
		value = malloc(2);
		if (!value)
			return (NULL);
		value[0] = '$';
		value[1] = '\0';
		return (value);
	}
	var_len = get_var_name_len(str + *i);
	if (var_len == 0)
	{
		value = malloc(2);
		if (!value)
			return (NULL);
		value[0] = '$';
		value[1] = '\0';
		return (value);
	}
	value = get_env_value(str + *i, var_len, shell);
	*i += var_len - 1;
	return (value);
}

char	*expand_variables(char *str, t_shell *shell)
{
	char			*result;
	t_quote_state	state;
	int				i;
	int				res_len;
	char			*temp;

	result = NULL;
	state.in_single = 0;
	state.in_double = 0;
	i = 0;
	res_len = 0;
	while (str[i])
	{
		update_quote_state(str[i], &state);
		if (str[i] == '$' && str[i + 1])
		{
			temp = process_variable(str, &i, shell, &state);
			if (!temp)
				return (result);
			result = append_str(result, temp, &i, &res_len);
		}
		else
		{
			temp = malloc(2);
			if (!temp)
			{
				free(result);
				return (NULL);
			}
			temp[0] = str[i];
			temp[1] = '\0';
			result = append_str(result, temp, &i, &res_len);
		}
		if (!result)
			return (NULL);
		i++;
	}
	if (!result)
		return (ft_strdup(""));
	return (result);
}
