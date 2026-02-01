/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 16:20:00 by mrio              #+#    #+#             */
/*   Updated: 2026/01/23 16:18:52 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_redir_type	get_redir_type(t_token_type type)
{
	if (type == TOKEN_REDIR_IN)
		return (REDIR_IN);
	else if (type == TOKEN_REDIR_OUT)
		return (REDIR_OUT);
	else if (type == TOKEN_REDIR_APPEND)
		return (REDIR_APPEND);
	return (REDIR_HEREDOC);
}

static int	validate_redir_filename(t_token *current)
{
	if (!current || current->type != TOKEN_WORD)
	{
		if (current)
			handle_syntax_error(current->value);
		else
			handle_syntax_error("newline");
		return (1);
	}
	return (0);
}

static void	set_heredoc_expand(t_redirect *redir, t_token *filename_token)
{
	if (filename_token->quote == QUOTE_NONE)
		redir->expand_heredoc = 1;
	else
		redir->expand_heredoc = 0;
}

int	handle_redirection(t_cmd *cmd, t_token **current)
{
	t_redirect		*redir;
	t_redir_type	type;
	char			*filename;
	t_token			*filename_token;

	type = get_redir_type((*current)->type);
	*current = (*current)->next;
	if (validate_redir_filename(*current))
		return (1);
	filename_token = *current;
	filename = ft_strdup((*current)->value);
	if (!filename)
		return (1);
	redir = create_redirect(type, filename);
	if (!redir)
		return (free(filename), 1);
	if (type == REDIR_HEREDOC)
		set_heredoc_expand(redir, filename_token);
	add_redir_back(&cmd->redir_list, redir);
	*current = (*current)->next;
	return (0);
}
