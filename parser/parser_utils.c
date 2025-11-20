/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: toshi <toshi@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 18:33:45 by toshi             #+#    #+#             */
/*   Updated: 2025/11/18 18:52:25 by toshi            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

// 新しい (空の) t_cmd ノードを作成する
t_cmd	*create_cmd(void)
{
	t_cmd	*new_cmd;

	new_cmd = (t_cmd *)malloc(sizeof(t_cmd));
	if (!new_cmd)
		return (NULL);
	new_cmd->args = NULL;
	new_cmd->redir_list = NULL;
	new_cmd->next = NULL;
	return (new_cmd);
}

// t_cmd リスト (パイプライン) の末尾に t_cmd を追加する
void	add_cmd_back(t_cmd **list, t_cmd *new_cmd)
{
	t_cmd	*current;

	if (!list || !new_cmd)
		return ;
	if (*list == NULL)
	{
		*list = new_cmd;
		return ;
	}
	current = *list;
	while (current->next != NULL)
		current = current->next;
	current->next = new_cmd;
}

// 新しい t_redirect ノードを作成する
t_redirect	*create_redirect(t_redir_type type, char *filename)
{
	t_redirect	*new_redir;

	new_redir = (t_redirect *)malloc(sizeof(t_redirect));
	if (!new_redir)
		return (NULL);
	new_redir->type = type;
	new_redir->filename = filename;
	new_redir->next = NULL;
	return (new_redir);
}

// t_redirect リストの末尾に t_redirect を追加する
void	add_redir_back(t_redirect **list, t_redirect *new_redir)
{
	t_redirect	*current;

	if (!list || !new_redir)
		return ;
	if (*list == NULL)
	{
		*list = new_redir;
		return ;
	}
	current = *list;
	while (current->next != NULL)
		current = current->next;
	current->next = new_redir;
}

// t_list (文字列のリスト) を NULL 終端の char** 配列に変換する
char	**convert_list_to_array(t_list *list)
{
	char	**array;
	int		size;
	int		i;
	t_list	*current;

	size = ft_lstsize(list);
	array = (char **)malloc(sizeof(char *) * (size + 1));
	if (!array)
		return (NULL);
	i = 0;
	current = list;
	while (i < size)
	{
		// content (strdupされた文字列) のポインタを配列に移す
		array[i] = (char *)current->content;
		current->content = NULL; // ft_lstclear で free させないため
		current = current->next;
		i++;
	}
	array[size] = NULL;
	return (array);
}
