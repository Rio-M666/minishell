/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: toyamagu <toyamagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/24 23:57:58 by toyamagu          #+#    #+#             */
/*   Updated: 2026/01/25 00:13:04 by toyamagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	update_env(t_shell *shell, char *old_pwd)
{
	char	new_pwd[1024];

	if (getcwd(new_pwd, sizeof(new_pwd)) == NULL)
	{
		perror("minishell: cd: getcwd");
		return (1);
	}
	if (ft_setenv("OLDPWD", old_pwd, shell) != 0)
		return (1);
	if (ft_setenv("PWD", new_pwd, shell) != 0)
		return (1);
	return (0);
}

int	ft_cd(char **args, t_shell *shell)
{
	char	*path;
    char old_pwd[1024];

	if (!args[1])
	{
		path = ft_getenv("HOME", shell);
		if (!path)
		{
			ft_putstr_fd("minishell: cd: HOME not set\n", 2);
			return (1);
		}
	}
	else
		path = args[1];
	if (chdir(path) != 0)
	{
		perror("minishell: cd");
		return (1);
	}
    
	return (0);
}
