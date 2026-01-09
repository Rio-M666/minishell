/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_debug.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 13:16:50 by mrio              #+#    #+#             */
/*   Updated: 2026/01/09 13:16:51 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_redir_list(t_redirect *list)
{
	t_redirect	*current;

	current = list;
	while (current)
	{
		if (current->type == REDIR_IN)
			printf("    < (%s)\n", current->filename);
		else if (current->type == REDIR_OUT)
			printf("    > (%s)\n", current->filename);
		else if (current->type == REDIR_APPEND)
			printf("    >> (%s)\n", current->filename);
		else if (current->type == REDIR_HEREDOC)
			printf("    << (%s)\n", current->filename);
		current = current->next;
	}
}

// 構築したパイプライン (AST) を表示する
void	print_pipeline(t_cmd *pipeline)
{
	t_cmd	*current_cmd;
	int		i;
	int		j;

	current_cmd = pipeline;
	i = 0;
	printf("\n--- PARSED PIPELINE ---\n");
	while (current_cmd)
	{
		printf("--- Command %d ---\n", i++);
		j = 0;
		if (current_cmd->args)
		{
			while (current_cmd->args[j])
			{
				printf("  Arg[%d]: [%s]\n", j, current_cmd->args[j]);
				j++;
			}
		}
		else
		{
			printf("  (No Args)\n");
		}
		printf("  Redirections:\n");
		print_redir_list(current_cmd->redir_list);
		current_cmd = current_cmd->next;
		if (current_cmd)
			printf("    | (PIPE)\n");
	}
	printf("-----------------------\n\n");
}
