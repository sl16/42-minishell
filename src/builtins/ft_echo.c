/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vbartos <vbartos@student.42prague.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 13:45:57 by vbartos           #+#    #+#             */
/*   Updated: 2024/01/14 18:31:00 by vbartos          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../incl/minishell.h"

// ft_echo
// - simulates the bash builtin 'echo';
// - also checks for the -n flag (doesn't print newline);
void ft_echo(char **args, t_data *data)
{
	int	n_flag;
	int	i;

	n_flag = 0;
	i = 1;
	if (strs_count(args) > 1)
	{
		if (args[1] && ft_strncmp(args[1], "-n", 2) == 0)
		{
			n_flag = 1;
			i++;
		}
		while (args[i] != NULL)
		{
			ft_putstr_fd(args[i], STDOUT_FILENO);
			if (args[i + 1] && args[i][0] != '\0')
				ft_putstr_fd(" ", STDOUT_FILENO);
			i++;
		}
	}
	if (n_flag == 0)
		ft_putstr_fd("\n", STDOUT_FILENO);
	data->exit_status = 0;
}
