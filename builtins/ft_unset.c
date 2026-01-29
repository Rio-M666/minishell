/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: toyamagu <toyamagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 00:59:35 by toyamagu          #+#    #+#             */
/*   Updated: 2026/01/26 01:09:21 by toyamagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_valid_key(char *str)
{
	int	i;

	i = 0;
	if (!str || !str[0] || ft_isdigit(str[0]))
		return (0);
	while (str[i])
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

int	ft_unset(char **args, t_shell *shell)
{
	int	i;
	int	status;

	status = 0;
	i = 1;
	while (args[i])
	{
		if (!is_valid_key(args[i]))
		{
			print_error_arg("unset", args[i], "not a valid identifier");
			status = 1;
		}
		else
			ft_unset_env(args[i], shell);
		i++;
	}
	return (status);
}
