/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulicna <aulicna@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/14 11:33:30 by vbartos           #+#    #+#             */
/*   Updated: 2024/01/22 13:29:31 by aulicna          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../incl/minishell.h"

/**
 * @brief Executes a list of simple commands.
 * 
 * Saves the STDIN and STDOUT file descriptors for later restoration.
 * If there is only one command and it's a builtin, runs it in the main process.
 * For all other cases, runs a pipeline with child processes.
 * 
 * @param data The data structure containing the shell's state and settings.
 * @param simple_cmds The linked list of simple commands to execute.
 */
int	exec(t_data *data, t_list *simple_cmds)
{
	t_simple_cmds	*content;
	int				cmds_num;

	orig_fds_save(&data->orig_fdin, &data->orig_fdout);
	content = (t_simple_cmds *) simple_cmds->content;
	cmds_num = ft_lstsize(simple_cmds);
	if (simple_cmds->next == NULL && is_builtin(content->cmd[0]))
	{
		if (content->redirects)
			handle_redirect(data, content->redirects, content->hd_file);
		run_builtin(data, content->cmd);
	}
	else
		exec_pipeline(data, simple_cmds, cmds_num);
	orig_fds_restore(data->orig_fdin, data->orig_fdout);
	return (0);
}

/**
 * @brief	Executes a pipeline of simple commands.
 *
 * This function takes a data structure, a linked list of simple commands,
 * and the number of commands in the pipeline. It creates pipes for
 * inter-process communication and forks child processes to execute each
 * command in the pipeline. The input and output file descriptors are managed
 * accordingly to redirect the input and output of each command.
 * The function waits for all child processes to complete before returning.
 *
 * Details: 
 * If the current command is not the first one, the parent process 
 * (after fork_cmd line) closes the read end of the previous pipe. Each command
 * reads from the previous pipe and writes to the current pipe, so after
 * a command has finished reading from a pipe, it should close the read end so
 * that the writing process can get a SIGPIPE signal if it tries to write
 * to the pipe.
 * 
 * @param data Pointer to the t_data structure (for simple_cmds, exit_status)
 * @param simple_cmds The linked list of simple commands in the pipeline.
 * @param cmds_num The number of commands in the pipeline.
 */
void	exec_pipeline(t_data *data, t_list *simple_cmds, int cmds_num)
{
	int	**fd_pipe;
	int	i;

	i = 0;
	fd_pipe = malloc(sizeof(int *) * (ft_lstsize(simple_cmds)));
	while (i < cmds_num)
	{
		fd_pipe[i] = malloc(sizeof(int) * 2);
		if (pipe(fd_pipe[i]) == -1)
		{
			ft_putendl_fd("minishell: pipe: Too many open files", 2);
			exit_current_prompt(NULL);
		}
		fork_cmd(data, simple_cmds, fd_pipe, i);
		close(fd_pipe[i][PIPE_WRITE]);
		if (i > 0)
			close(fd_pipe[i - 1][PIPE_READ]);
		simple_cmds = simple_cmds->next;
		i++;
	}
	wait_for_pipeline(data, cmds_num, fd_pipe, i);
	free_pipe(fd_pipe, ft_lstsize(data->simple_cmds));
}

/**
 * @brief	Forks a new process to execute a command.
 * 
 * pipe_redirect takes care of redirecting input and output fds for piping.
 * 
 * handle_redirect checks for redirections and redirects input/ output
 * accordingly.
 * 
 * free_pipe_child solves still reachable leaks in the child process since
 * it inherits the malloced fd_pipe.
 * 
 * Then the command is executed.
 *
 * @param	data		pointer to the t_data structure (for exit_status)
 * @param	simple_cmds	list of simple commands to execute
 * @param	fd_pipe		2d array of file descriptors
 * @param	i			current position in fd_pipe
 * @return	int			process ID of the child process
 */
int	fork_cmd(t_data *data, t_list *simple_cmds, int **fd_pipe, int i)
{
	int				pid;
	t_simple_cmds	*content;

	content = (t_simple_cmds *)simple_cmds->content;
	pid = fork();
	if (pid == -1)
	{
		ft_putendl_fd("minishell: fork: Resource temporarily unavailable", 2);
		exit_current_prompt(NULL);
	}
	if (pid == 0)
	{
		pipe_redirect(simple_cmds, fd_pipe, i);
		if (content->redirects)
			handle_redirect(data, content->redirects, content->hd_file);
		free_pipe_child(fd_pipe, i);
		if (is_builtin(content->cmd[0]))
		{
			run_builtin(data, content->cmd);
			exit_minishell(NULL, 0);
		}
		else
			run_exec(data, content);
	}
	return (pid);
}

/**
 * @brief	Executes the given command. First checks if the command was given
 * in and absolute path. If so, executes it. If not, parses the command name and
 * attempts to generate a path for the executable.
 * 
 * @param data The data structure containing the shell's state.
 * @param content The structure containing the parsed command.
 */
void	run_exec(t_data *data, t_simple_cmds *content)
{
	char	**env_cpy;
	char	*path;

	env_cpy = env_copy(data);
	if (access(content->cmd[0], F_OK) == 0)
		execve(content->cmd[0], content->cmd, env_cpy);
	path = find_exe_path(data, content->cmd[0]);
	if (path != NULL)
	{
		execve(path, content->cmd, env_cpy);
		free(path);
	}
	else if (path == NULL)
	{
		free(env_cpy);
		ft_putstr_fd(content->cmd[0], STDERR);
		ft_putendl_fd(": command not found", STDERR);
		exit_minishell(NULL, 127);
	}
	free(env_cpy);
	exit_minishell(NULL, 126);
}

/**
 * @brief	Executes the appropriate built-in command based on the given command.
 *
 * @param data The data structure containing the shell's state.
 * @param cmd The command to be executed.
 */
void	run_builtin(t_data *data, char **cmd)
{
	if (ft_strncmp(cmd[0], "cd", 2) == 0)
		ft_cd(cmd, data);
	else if (ft_strncmp(cmd[0], "echo", 4) == 0)
		ft_echo(cmd, data);
	else if (ft_strncmp(cmd[0], "env", 3) == 0)
		ft_env(data);
	else if (ft_strncmp(cmd[0], "exit", 4) == 0)
		ft_exit(cmd, data);
	else if (ft_strncmp(cmd[0], "export", 6) == 0)
		ft_export(cmd, data);
	else if (ft_strncmp(cmd[0], "pwd", 3) == 0)
		ft_pwd(data);
	else if (ft_strncmp(cmd[0], "unset", 5) == 0)
		ft_unset(cmd, data);
}