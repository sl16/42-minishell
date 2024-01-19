/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_helpers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulicna <aulicna@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 14:27:08 by vbartos           #+#    #+#             */
/*   Updated: 2024/01/13 16:41:33 by aulicna          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../incl/minishell.h"

int	free_array(char **strs)
{
	int	i;

	i = 0;
	while (strs[i] != NULL)
	{
		free(strs[i]);
		i++;
	}
	free(strs);
	return (0);
}

int	free_envlist(t_list **head)
{
	t_list	*current;
	t_list	*next;
	t_env	*env;

	current = *head;
	while (current != NULL)
	{
		next = current->next;
		env = (t_env *) current->content;
		free(env->full_string);
		free(env->name);
		free(env->value);
		free(current->content);
		free(current);
		current = next;
	}
	*head = NULL;
	return (0);
}

void	free_lexer(t_list **lexer)
{
	t_list	*tmp;
	t_lexer	*content;

	if (!*lexer)
		return ;
	while (*lexer)
	{
		tmp = (*lexer)->next;
		content = (t_lexer *)(*lexer)->content;
		if (content->word)
			free(content->word);
		free(content);
		free(*lexer);
		*lexer = tmp;
	}
	lexer = NULL;
}

void	free_simple_cmds(t_list **simple_cmds)
{
	t_simple_cmds	*content;
	t_list			*free_redirects;
	t_list			*tmp;

	if (!simple_cmds)
		return ;
	while (*simple_cmds)
	{
		tmp = (*simple_cmds)->next;
		content = (t_simple_cmds *)(*simple_cmds)->content;
		free_redirects = content->redirects;
		free_lexer(&free_redirects);
		free_array(content->cmd);
		free(content->hd_file);
		free(content);
		free(*simple_cmds);
		*simple_cmds = tmp;
	}
	simple_cmds = NULL;
}

void	free_struct_str(t_str *str, char *old_str)
{
	if (str->part_1)
		free(str->part_1);
	if (str->part_2)
		free(str->part_2);
	if (str->part_3)
		free(str->part_3);
	if (str->tmp_join)
		free(str->tmp_join);
	free(old_str);
}
