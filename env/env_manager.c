/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_manager.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: toyamagu <toyamagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/24 23:50:08 by toyamagu          #+#    #+#             */
/*   Updated: 2026/01/28 15:52:21 by toyamagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	count_env(char **envp)
{
	int	i;

	i = 0;
	if (!envp)
		return (0);
	while (envp[i])
		i++;
	return (i);
}

static int	add_env_entry(t_shell *shell, char *new_entry)
{
	int		i;
	char	**new_envp;

	i = count_env(shell->envp);
	new_envp = malloc(sizeof(char *) * (i + 2));
	if (!new_envp)
		return (free(new_entry), 1);
	i = 0;
	while (shell->envp && shell->envp[i])
	{
		new_envp[i] = shell->envp[i];
		i++;
	}
	new_envp[i] = new_entry;
	new_envp[i + 1] = NULL;
	if (shell->envp)
		free(shell->envp);
	shell->envp = new_envp;
	return (0);
}

char	*ft_getenv(char *key, t_shell *shell)
{
	int		i;
	size_t	len;

	if (!key || !shell->envp)
		return (NULL);
	len = ft_strlen(key);
	i = 0;
	while (shell->envp[i])
	{
		if (ft_strncmp(shell->envp[i], key, len) == 0
			&& shell->envp[i][len] == '=')
		{
			return (shell->envp[i] + len + 1);
		}
		i++;
	}
	return (NULL);
}

int	ft_setenv(char *key, char *value, t_shell *shell)
{
	int		i;
	char	*str;
	char	*new_entry;
	size_t	len;

	str = ft_strjoin(key, "=");
	new_entry = ft_strjoin(str, value);
	free(str);
	if (!new_entry)
		return (1);
	len = ft_strlen(key);
	i = 0;
	while (shell->envp && shell->envp[i])
	{
		if (ft_strncmp(shell->envp[i], key, len) == 0
			&& shell->envp[i][len] == '=')
		{
			free(shell->envp[i]);
			shell->envp[i] = new_entry;
			return (0);
		}
		i++;
	}
	return (add_env_entry(shell, new_entry));
}

void	ft_unset_env(char *key, t_shell *shell)
{
	int		i;
	int		j;
	size_t	len;

	if (!key || !shell->envp)
		return ;
	len = ft_strlen(key);
	i = 0;
	while (shell->envp[i])
	{
		if (ft_strncmp(shell->envp[i], key, len) == 0
			&& (shell->envp[i][len] == '=' || shell->envp[i][len] == '\0'))
		{
			free(shell->envp[i]);
			j = i;
			while (shell->envp[j + 1])
			{
				shell->envp[j] = shell->envp[j + 1];
				j++;
			}
			shell->envp[j] = NULL;
			return ;
		}
		i++;
	}
}

char	**init_envp(char **envp)
{
	int i;
	int count;
	char **new_envp;

	count = count_env(envp);
	new_envp = malloc(sizeof(char *) * (count + 1));
	if (!new_envp)
		return (NULL);
	i = 0;
	while (i < count)
	{
		new_envp[i] = ft_strdup(envp[i]);
		if (!new_envp[i])
		{
			free_array(new_envp);
			return (NULL);
		}
		i++;
	}
	new_envp[i] = NULL;
	return (new_envp);
}
