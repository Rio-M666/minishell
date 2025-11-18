#include "minishell.h"

// 新しい t_list ノードを作成する
t_list	*ft_lstnew(void *content)
{
	t_list	*new_node;

	new_node = (t_list *)malloc(sizeof(t_list));
	if (!new_node)
		return (NULL);
	new_node->content = content;
	new_node->next = NULL;
	return (new_node);
}

// t_list の末尾にノードを追加する
void	ft_lstadd_back(t_list **lst, t_list *new_node)
{
	t_list	*current;

	if (!lst || !new_node)
		return ;
	if (*lst == NULL)
	{
		*lst = new_node;
		return ;
	}
	current = *lst;
	while (current->next != NULL)
		current = current->next;
	current->next = new_node;
}

// t_list のサイズを返す
int	ft_lstsize(t_list *lst)
{
	int		count;
	t_list	*current;

	count = 0;
	current = lst;
	while (current != NULL)
	{
		count++;
		current = current->next;
	}
	return (count);
}

// t_list 全体を解放する (del 関数で content も解放)
void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*current;
	t_list	*tmp;

	if (!lst)
		return ;
	current = *lst;
	while (current != NULL)
	{
		tmp = current->next;
		if (del && current->content)
			del(current->content);
		free(current);
		current = tmp;
	}
	*lst = NULL;
}
