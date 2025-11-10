#include "minishell.h"

size_t	ft_strlen(const char *str)
{
	size_t	i;

	i = 0;
	while (str[i] != '\0')
	{
		i++;
	}
	return (i);
}
void	ft_putstr_fd(char *s, int fd)
{
	if (s == NULL)
		return ;
	while (*s)
	{
		write(fd, s, 1);
		s++;
	}
}

char	*ft_strdup(const char *s)
{
	const char	*copy;
	char		*dest;
	int			len;
	int			i;

	copy = s;
	len = 0;
	while (*s)
	{
		s++;
		len++;
	}
	dest = (char *)malloc((len + 1) * sizeof(char));
	if (dest == NULL)
		return (NULL);
	i = 0;
	while (i < len)
	{
		dest[i] = copy[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	i;
	size_t	s_len;
	size_t	sub_len;
	char	*str;

	if (!s)
		return (NULL);
	s_len = ft_strlen(s);
	if (start >= s_len)
		return (ft_strdup(""));
	sub_len = s_len - start;
	if (sub_len > len)
		sub_len = len;
	str = (char *)malloc(sizeof(char) * (sub_len + 1));
	if (!str)
		return (NULL);
	i = 0;
	while (i < sub_len)
	{
		str[i] = s[start + i];
		i++;
	}
	str[i] = '\0';
	return (str);
}
char	*ft_strchr(const char *str, int c)
{
	size_t			i;
	unsigned char	ch;

	i = 0;
	ch = (unsigned char)c;
	while (str[i] != '\0')
	{
		if ((unsigned char)str[i] == ch)
		{
			return ((char *)(str + i));
		}
		i++;
	}
	if (ch == '\0')
		return ((char *)(str + i));
	return (NULL);
}

size_t	word_count(char const *s, char c)
{
	size_t	count;

	count = 0;
	while (*s)
	{
		while (*s == c)
			s++;
		if (*s)
			count++;
		while (*s != c && *s)
			s++;
	}
	return (count);
}

void	ft_free(char **str, int count)
{
	while (count > 0)
	{
		count--;
		free(str[count]);
	}
	free(str);
}

char	**split_sub(char **str, char const *s, char c)
{
	size_t	len;
	int		i;

	i = 0;
	while (*s)
	{
		while (*s == c && *s)
			s++;
		if (!*s)
			break ;
		if (!ft_strchr(s, c))
			len = ft_strlen(s);
		else
			len = ft_strchr(s, c) - s;
		str[i] = ft_substr(s, 0, len);
		if (!str[i++])
		{
			ft_free(str, i - 1);
			return (NULL);
		}
		s += len;
	}
	str[i] = NULL;
	return (str);
}
char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	i;
	size_t	j;
	size_t	k;
	char	*str;

	i = ft_strlen(s1);
	j = ft_strlen(s2);
	str = (char *)malloc(i + j + 1);
	if (!str)
		return (NULL);
	k = 0;
	while (*s1)
		str[k++] = *s1++;
	while (*s2)
		str[k++] = *s2++;
	str[k] = '\0';
	return (str);
}
void	free_array(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	while (arr[i])
		free(arr[i++]);
	free(arr);
}

char	**ft_split(char const *s, char c)
{
	char	**str;
	int		i;

	if (!s)
		return (NULL);
	i = 0;
	str = malloc((word_count(s, c) + 1) * sizeof(char *));
	if (!str)
		return (NULL);
	return (split_sub(str, s, c));
}
