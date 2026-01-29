/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/24 23:57:58 by toyamagu          #+#    #+#             */
/*   Updated: 2026/01/28 20:59:45 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	update_pwd(t_shell *shell)
{
	char	cwd[PATH_MAX];
	char	*oldpwd;

	oldpwd = ft_getenv("PWD", shell);
	if (oldpwd)
		ft_setenv("OLDPWD", oldpwd, shell);
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		ft_setenv("PWD", cwd, shell);
	return (0);
}

int	ft_cd(char **args, t_shell *shell)
{
	char	*path;

	if (!args[1])
	{
		path = ft_getenv("HOME", shell);
		if (!path)
		{
			ft_putstr_fd("minishell: cd: HOME not set\n", 2);
			return (1);
		}
	}
	else if (args[2])
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", 2);
		return (1);
	}
	else
		path = args[1];
	if (chdir(path) != 0)
	{
		perror("minishell: cd");
		return (1);
	}
	update_pwd(shell);
	return (0);
}
