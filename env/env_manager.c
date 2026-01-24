/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_manager.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: toyamagu <toyamagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/24 23:50:08 by toyamagu          #+#    #+#             */
/*   Updated: 2026/01/24 23:57:09 by toyamagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	**init_envp(char **envp)
{
	int		i;
	int		count;
	char	**new_envp;

	count = 0;
	while (envp[count])
		count++;
	new_envp = malloc(sizeof(char *) * (count + 1));
	if (!new_envp)
		return (NULL);
	i = 0;
	while (i < count)
	{
		new_envp[i] = ft_strdup(envp[i]);
		if (!new_envp[i])
			return (free_array(new_envp), NULL);
		i++;
	}
	new_envp[i] = NULL;
	return (new_envp);
}

char	*ft_getenv(char *key, t_shell *shell)
{
	int		i;
	size_t	len;
	char	*env_key;

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
	char *new_entry;
	char *tmp;
	int i;
	size_t len;

	tmp = ft_strjoin(key, "=");
	new_entry = ft_strjoin(tmp, value);
	free(tmp);
	if (!new_entry)
		return (1);
	len = ft_strlen(key);
	i = 0;
	while (shell->envp[i])
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
	return (add_env_variable(shell, new_entry)); 
}
