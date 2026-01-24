/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 16:19:52 by mrio              #+#    #+#             */
/*   Updated: 2026/01/23 16:21:48 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_token	*process_word_token(char **input)
{
	char			*raw_word;
	char			*clean_word;
	t_quote_type	quote_type;
	t_token			*new_token;

	raw_word = get_word(input);
	if (!raw_word)
		return (NULL);
	quote_type = get_quote_type(raw_word);
	clean_word = strip_quotes(raw_word);
	free(raw_word);
	if (!clean_word)
		return (NULL);
	new_token = create_token(TOKEN_WORD, clean_word);
	free(clean_word);
	if (!new_token)
		return (NULL);
	new_token->quote = quote_type;
	return (new_token);
}

t_token	*tokenize(char *input)
{
	t_token	*tokens;
	t_token	*new_token;

	tokens = NULL;
	while (*input)
	{
		skip_space(&input);
		if (!*input)
			break ;
		if (is_special(*input))
		{
			new_token = tokenize_special(&input);
			if (!new_token)
				return (free_tokens(tokens), NULL);
			new_token->quote = QUOTE_NONE;
		}
		else
		{
			new_token = process_word_token(&input);
			if (!new_token)
				return (free_tokens(tokens), NULL);
		}
		add_token(&tokens, new_token);
	}
	return (tokens);
}
