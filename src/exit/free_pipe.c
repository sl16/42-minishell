/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulicna <aulicna@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 13:17:14 by aulicna           #+#    #+#             */
/*   Updated: 2024/01/22 13:30:27 by aulicna          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../incl/minishell.h"

void	free_pipe(int **fd_pipe, int num_cmds)
{
	int	i;

	i = 0;
	while (i < num_cmds)
	{
		free(fd_pipe[i]);
		i++;
	}
	free(fd_pipe);
}

void	free_pipe_child(int **fd_pipe, int i)
{
	free(fd_pipe[i]);
	free(fd_pipe);
}