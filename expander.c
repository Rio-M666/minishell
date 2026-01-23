#include "minishell.h"

/* $? を終了ステータスの数値文字列に変換 */
static char	*get_exit_status(int status)
{
	char	*str;

	str = malloc(12);
	if (!str)
		return (ft_strdup(""));
	snprintf(str, 12, "%d", status);
	return (str);
}

/* $VAR_NAME の長さを返す ($?は1, 無効なら0) */
static int	var_name_len(char *s)
{
	int	len;

	if (s[0] == '?')
		return (1);
	if (!ft_isalpha(s[0]) && s[0] != '_')
		return (0);
	len = 0;
	while (s[len] && (ft_isalnum(s[len]) || s[len] == '_'))
		len++;
	return (len);
}

/* 変数名から値を取得 (なければ空文字列) */
static char	*get_var_value(char *name, int len, t_shell *shell)
{
	char	*var;
	char	*val;

	if (len == 1 && name[0] == '?')
		return (get_exit_status(shell->last_status));
	var = ft_substr(name, 0, len);
	if (!var)
		return (ft_strdup(""));
	val = getenv(var);
	free(var);
	if (!val)
		return (ft_strdup(""));
	return (ft_strdup(val));
}

/*
 * 文字列中の$変数を展開する
 * シングルクォート内では展開しない
 */
char	*expand_variables(char *str, t_shell *shell)
{
	char	*result;
	char	*tmp;
	char	*val;
	int		i;
	int		in_single;
	int		in_double;
	int		var_len;

	result = ft_strdup("");
	in_single = 0;
	in_double = 0;
	i = 0;
	while (str[i])
	{
		/* クォート状態の追跡 */
		if (str[i] == '\'' && !in_double)
			in_single = !in_single;
		else if (str[i] == '"' && !in_single)
			in_double = !in_double;
		/* $変数の展開 (シングルクォート外のみ) */
		if (str[i] == '$' && str[i + 1] && !in_single)
		{
			var_len = var_name_len(str + i + 1);
			if (var_len > 0)
			{
				val = get_var_value(str + i + 1, var_len, shell);
				tmp = ft_strjoin(result, val);
				free(result);
				free(val);
				result = tmp;
				i += var_len + 1;
				continue ;
			}
		}
		/* 通常の文字を追加 */
		tmp = malloc(ft_strlen(result) + 2);
		if (tmp)
		{
			int j = 0;
			while (result[j])
			{
				tmp[j] = result[j];
				j++;
			}
			tmp[j] = str[i];
			tmp[j + 1] = '\0';
		}
		free(result);
		result = tmp;
		i++;
	}
	return (result);
}

/*
 * 全トークンの変数を展開
 * シングルクォートのWORDとheredocのデリミタはスキップ
 */
void	expand_tokens(t_token *tokens, t_shell *shell)
{
	char	*expanded;

	while (tokens)
	{
		if (tokens->type == TOKEN_WORD && tokens->quote != QUOTE_SINGLE)
		{
			expanded = expand_variables(tokens->value, shell);
			if (expanded)
			{
				free(tokens->value);
				tokens->value = expanded;
			}
		}
		/* heredocのデリミタはスキップ(次のトークン) */
		if (tokens->type == TOKEN_REDIR_HEREDOC && tokens->next)
			tokens = tokens->next;
		tokens = tokens->next;
	}
}
