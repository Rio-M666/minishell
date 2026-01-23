/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 18:33:49 by toshi             #+#    #+#             */
/*   Updated: 2026/01/23 16:18:50 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	add_word_to_list(t_list **arg_list, t_token *current)
{
	char	*arg_val;
	t_list	*new_node;

	arg_val = ft_strdup(current->value);
	new_node = ft_lstnew(arg_val);
	if (!arg_val || !new_node)
		return (free(arg_val), 0);
	ft_lstadd_back(arg_list, new_node);
	return (1);
}

static void	cleanup_parse_cmd(t_list **arg_list, t_cmd *cmd)
{
	ft_lstclear(arg_list, free);
	free_pipeline(cmd);
}

static t_cmd	*parse_simple_command(t_token **tokens)
{
	t_cmd	*cmd;
	t_list	*arg_list;
	t_token	*current;

	cmd = create_cmd();
	arg_list = NULL;
	current = *tokens;
	while (current && current->type != TOKEN_PIPE)
	{
		if (current->type == TOKEN_WORD)
		{
			if (!add_word_to_list(&arg_list, current))
				return (cleanup_parse_cmd(&arg_list, cmd), NULL);
			current = current->next;
		}
		else if (handle_redirection(cmd, &current) == 0)
			return (cleanup_parse_cmd(&arg_list, cmd), NULL);
	}
	cmd->args = convert_list_to_array(arg_list);
	ft_lstclear(&arg_list, NULL);
	*tokens = current;
	return (cmd);
}

static int	advance_past_pipe(t_token **current)
{
	*current = (*current)->next;
	if (*current == NULL || (*current)->type == TOKEN_PIPE)
	{
		if (*current)
			handle_syntax_error((*current)->value);
		else
			handle_syntax_error("newline");
		return (0);
	}
	return (1);
}

t_cmd	*parse(t_token *tokens)
{
	t_cmd	*pipeline_head;
	t_cmd	*new_cmd;
	t_token	*current;

	if (!tokens)
		return (NULL);
	current = tokens;
	pipeline_head = NULL;
	if (current->type == TOKEN_PIPE)
		return (handle_syntax_error(current->value), NULL);
	while (current != NULL)
	{
		new_cmd = parse_simple_command(&current);
		if (!new_cmd)
			return (free_pipeline(pipeline_head), NULL);
		add_cmd_back(&pipeline_head, new_cmd);
		if (current && current->type == TOKEN_PIPE)
			if (!advance_past_pipe(&current))
				return (free_pipeline(pipeline_head), NULL);
	}
	return (pipeline_head);
}
