/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 17:27:36 by mrio              #+#    #+#             */
/*   Updated: 2025/11/23 06:35:28 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	expand_tokens(t_token *tokens, t_shell *shell)
{
	t_token	*current;
	char	*expanded;

	current = tokens;
	while (current)
	{
		if (current->type == TOKEN_WORD && current->quote != QUOTE_SINGLE)
		{
			expanded = expand_variables(current->value, shell, current->quote);
			if (expanded)
			{
				free(current->value);
				current->value = expanded;
			}
		}
		else if (current->type == TOKEN_REDIR_HEREDOC)
		{
			current = current->next;
			continue ;
		}
		current = current->next;
	}
}
