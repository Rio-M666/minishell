/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_free.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 15:46:38 by mrio              #+#    #+#             */
/*   Updated: 2026/01/23 15:46:40 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// t_redirect リストを解放する
static void	free_redir_list(t_redirect *list)
{
	t_redirect	*current;
	t_redirect	*tmp;

	current = list;
	while (current != NULL)
	{
		tmp = current->next;
		free(current->filename);
		free(current);
		current = tmp;
	}
}

// 1つの t_cmd ノードを解放する
static void	free_cmd(t_cmd *cmd)
{
	if (!cmd)
		return ;
	free_array(cmd->args);
	free_redir_list(cmd->redir_list);
	free(cmd);
}

// パイプライン (t_cmd リスト) 全体を解放する
void	free_pipeline(t_cmd *pipeline)
{
	t_cmd	*current;
	t_cmd	*tmp;

	current = pipeline;
	while (current != NULL)
	{
		tmp = current->next;
		free_cmd(current);
		current = tmp;
	}
}
