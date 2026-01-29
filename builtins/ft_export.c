/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: toyamagu <toyamagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/25 00:35:44 by toyamagu          #+#    #+#             */
/*   Updated: 2026/01/26 01:09:30 by toyamagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_valid_identifier(char *str)
{
	int	i;

	i = 0;
	if (!str || !str[0] || ft_isdigit(str[0]) || str[0] == '=')
		return (0);
	while (str[i] && str[i] != '=')
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

static void	print_export_error(char *arg)
{
	print_error_arg("export", arg, "not a valid identifier");
}

static int	process_export_arg(char *arg, t_shell *shell)
{
	char	*key;
	char	*eq_pos;

	if (!is_valid_identifier(arg))
	{
		print_export_error(arg);
		return (1);
	}
	eq_pos = ft_strchr(arg, '=');
	if (eq_pos)
	{
		key = ft_substr(arg, 0, eq_pos - arg);
		if (!key)
			return (1);
		ft_setenv(key, eq_pos + 1, shell);
		free(key);
	}
	return (0);
}

int	ft_export(char **args, t_shell *shell)
{
	int	i;
	int	status;

	status = 0;
	if (!args[1])
		return (ft_env(shell));
	i = 1;
	while (args[i])
	{
		if (process_export_arg(args[i], shell) != 0)
			status = 1;
		i++;
	}
	return (status);
}
