/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrio <mrio@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 13:15:57 by mrio              #+#    #+#             */
/*   Updated: 2026/01/09 13:15:58 by mrio             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*tokenize(char *input)
{
	t_token			*tokens;
	t_token			*new_token;
	char			*raw_word;
	char			*clean_word;
	t_quote_type	quote_type;

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
			add_token(&tokens, new_token);
		}
		else
		{
			raw_word = get_word(&input);
			if (!raw_word)
				return (free_tokens(tokens), NULL);
			quote_type = get_quote_type(raw_word);
			clean_word = strip_quotes(raw_word);
			free(raw_word);
			if (!clean_word)
				return (free_tokens(tokens), NULL);
			new_token = create_token(TOKEN_WORD, clean_word);
			free(clean_word);
			if (!new_token)
				return (free_tokens(tokens), NULL);
			new_token->quote = quote_type;
			add_token(&tokens, new_token);
		}
	}
	return (tokens);
}
