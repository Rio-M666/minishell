/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: toshi <toshi@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 18:33:49 by toshi             #+#    #+#             */
/*   Updated: 2025/11/18 20:41:27 by toshi            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

static int	handle_redirection(t_cmd *cmd, t_token **current)
{
	t_redirect		*redir;
	t_redir_type	type;
	char			*filename;
	t_token			*filename_token;

	if ((*current)->type == TOKEN_REDIR_IN)
		type = REDIR_IN;
	else if ((*current)->type == TOKEN_REDIR_OUT)
		type = REDIR_OUT;
	else if ((*current)->type == TOKEN_REDIR_APPEND)
		type = REDIR_APPEND;
	else
		type = REDIR_HEREDOC;
	*current = (*current)->next;
	if (!*current || (*current)->type != TOKEN_WORD)
	{
		handle_syntax_error((*current) ? (*current)->value : "newline");
		return (0);
	}
	filename_token = *current;
	filename = ft_strdup((*current)->value);
	if (!filename)
		return (0); // Malloc error
	redir = create_redirect(type, filename);
	if (!redir)
		return (free(filename), 0); // Malloc error
	if (type == REDIR_HEREDOC)
	{
		if (filename_token->quote == QUOTE_NONE)
			redir->expand_heredoc = 1;
		else
			redir->expand_heredoc = 0;
	}
	add_redir_back(&cmd->redir_list, redir);
	*current = (*current)->next; // ファイル名を消費
	return (1); // 成功
}

// 1つの単純なコマンド (パイプ間の1セグメント) を解析する
static t_cmd	*parse_simple_command(t_token **tokens)
{
	t_cmd	*cmd;
	t_list	*arg_list;
	t_token	*current;
	char	*arg_val;
	t_list	*new_node;

	cmd = create_cmd();
	arg_list = NULL;
	current = *tokens;
	while (current && current->type != TOKEN_PIPE)
	{
		if (current->type == TOKEN_WORD)
		{
			arg_val = ft_strdup(current->value);
			new_node = ft_lstnew(arg_val);
			if (!arg_val || !new_node)
				return (free(arg_val), free_pipeline(cmd), NULL); // Malloc error
			ft_lstadd_back(&arg_list, new_node);
			current = current->next;
		}
		else if (handle_redirection(cmd, &current) == 0) // < > >> <<
		{
			ft_lstclear(&arg_list, free);
			free_pipeline(cmd);
			return (NULL); // 構文エラー
		}
	}
	cmd->args = convert_list_to_array(arg_list);
	ft_lstclear(&arg_list, NULL); // リスト構造のみ解放 (中身は cmd->args が保持)
	*tokens = current; // 呼び出し元のトークンポインタを更新
	return (cmd);
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
			return (free_pipeline(pipeline_head), NULL); // Malloc or Syntax error
		add_cmd_back(&pipeline_head, new_cmd);
		if (current && current->type == TOKEN_PIPE)
		{
			current = current->next; // パイプを消費
			if (current == NULL || current->type == TOKEN_PIPE)
			{
				handle_syntax_error(current ? current->value : "newline");
				return (free_pipeline(pipeline_head), NULL);
			}
		}
	}
	return (pipeline_head);
}
