/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_special.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 16:22:00 by mrio              #+#    #+#             */
/*   Updated: 2026/01/23 16:22:00 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_token	*tokenize_redir_in(char **str)
{
	if (*(*str + 1) == '<')
	{
		(*str) += 2;
		return (create_token(TOKEN_REDIR_HEREDOC, "<<"));
	}
	(*str)++;
	return (create_token(TOKEN_REDIR_IN, "<"));
}

static t_token	*tokenize_redir_out(char **str)
{
	if (*(*str + 1) == '>')
	{
		(*str) += 2;
		return (create_token(TOKEN_REDIR_APPEND, ">>"));
	}
	(*str)++;
	return (create_token(TOKEN_REDIR_OUT, ">"));
}

t_token	*tokenize_special(char **str)
{
	if (**str == '|')
	{
		(*str)++;
		return (create_token(TOKEN_PIPE, "|"));
	}
	else if (**str == '<')
		return (tokenize_redir_in(str));
	else if (**str == '>')
		return (tokenize_redir_out(str));
	return (NULL);
}
