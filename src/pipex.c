/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 13:47:54 by maweiss           #+#    #+#             */
/*   Updated: 2024/06/14 19:15:13 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

int	ft_first_child(t_pipex *pipex)
{
	int		fdin;
	char	*cmdpath;
	int		err;

	fdin = open(pipex->infile, O_RDONLY);
	if (fdin < 0)
	{
		ft_cleanup(pipex);
		exit(errno);
	}
	dup2(fdin, STDIN_FILENO);
	dup2(pipex->pipe[1][1], STDOUT_FILENO);
	close(fdin);
	ft_close_all_fds(pipex);
	cmdpath = ft_search_cmd(pipex, 1);
	if (cmdpath == NULL)
		err = 127;
	else
		err = execve(cmdpath, pipex->cmd_args[0], pipex->envp);
	ft_cleanup(pipex);
	exit(err);
}

int	ft_child(t_pipex *pipex, int nb_cmd)
{
	char	*cmdpath;
	int		err;

	dup2(pipex->pipe[(nb_cmd - 1) & 1][0], STDIN_FILENO);
	dup2(pipex->pipe[nb_cmd & 1][1], STDOUT_FILENO);
	ft_close_all_fds(pipex);
	cmdpath = ft_search_cmd(pipex, nb_cmd);
	if (cmdpath == NULL)
		err = 127;
	else
		err = execve(cmdpath, pipex->cmd_args[nb_cmd - 1], pipex->envp);
	ft_cleanup(pipex);
	exit(err);
}

int	ft_parent_process(t_pipex *pipex)
{
	char	*cmdpath;
	int		fdout;
	int		err;

	if (pipex->mode == here_doc)
		fdout = open(pipex->outfile, O_CREAT | O_APPEND | O_WRONLY, 0644);
	else
		fdout = open(pipex->outfile, O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (fdout < 0)
	{
		ft_cleanup(pipex);
		exit(errno);
	}
	dup2(fdout, STDOUT_FILENO);
	close(fdout);
	dup2(pipex->pipe[(pipex->nb_cmds -1) & 1][0], STDIN_FILENO);
	ft_close_all_fds(pipex);
	cmdpath = ft_search_cmd(pipex, pipex->nb_cmds);
	if (cmdpath == NULL)
		err = 127;
	else
		err = execve(cmdpath, pipex->cmd_args[pipex->nb_cmds - 1], pipex->envp);
	ft_cleanup(pipex);
	exit(err);
}

/* 	main function of project pipex.
	Input syntax: ./pipex infile cmd1 ... cmdn outfile
	alternative: ./pipex here_doc LIMITER cmd1 cmd2 outfile
*/
int	main(int argc, char **argv, char **envp)
{
	t_pipex	pipex;
	int		pid_n;
	int		i;

	ft_init_env(&pipex, &argc, argv, envp);
	ft_pipex_init(&pipex);
	i = 0;
	pipex.child_pids[i] = fork();
	if (pipex.child_pids[i] < 0)
	{
		strerror(10);
		return (10);
	}
	else if (pipex.child_pids[i] == 0)
	{
		if (pipex.mode == here_doc)
			ft_here_doc(&pipex);
		else
			ft_first_child(&pipex);
	}
	else
	{
		pid_n = -5;
		while (pipex.child_pids[i++] != 0 && i < pipex.nb_cmds - 1)
		{
			pipex.child_pids[i] = fork();
			if (pipex.child_pids[i] == 0)
				ft_child(&pipex, i + 1);
			if (close(pipex.pipe[(i) & 1][0])
				|| close(pipex.pipe[(i) & 1][1]))
				perror("pipex");
			pipe(pipex.pipe[(i) & 1]);
		}
		if (pid_n != 0)
		{
			pid_n = fork();
			pipex.child_pids[i] = pid_n;
			if (pid_n == 0)
				ft_parent_process(&pipex);
			else
			{
				ft_close_all_fds(&pipex);
				if (pipex.mode == here_doc)
				{
					unlink(TEMP_FILE);
					if (!access(TEMP_FILE, F_OK))
						ft_printf_err("pipex: could not delete tempfile\n");
				}
				ft_wait_error(&pipex);
			}
		}
	}
}
