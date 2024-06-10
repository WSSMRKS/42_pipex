/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 13:47:54 by maweiss           #+#    #+#             */
/*   Updated: 2024/06/10 23:23:52 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

char	*ft_search_cmd(t_pipex *pipex, int nbcmd)
{
	int		i;
	char	*path;
	int		sux;

	i = 0;
	sux = 0;
	while (pipex->path[i])
	{
		path = ft_strjoin(pipex->path[i], pipex->cmds[nbcmd - 1]);
		if (path == NULL)
		{
			perror("malloc fail!\n");
			exit(4);
		}
		if (access(path, X_OK) == 0)
		{
			sux = 1;
			break ;
		}
		free(path);
		i++;
	}
	if (sux == 1)
		return (path);
	else
		return (NULL);
}


int	ft_first_child(t_pipex *pipex)
{
	int		fdin;
	char	*cmdpath;

	fdin = open(pipex->infile, O_RDONLY);
	if (fdin < 0)
	{
		pipex->cmd_ret[0] = errno;
		ft_errhandle(pipex, 0);
		ft_cleanup(pipex);
		exit (1);
	}
	dup2(fdin, STDIN_FILENO);
	dup2(pipex->pipe[1][1], STDOUT_FILENO);
	close(fdin);
	ft_close_all_fds(pipex);
	cmdpath = ft_search_cmd(pipex, 1);
	if (cmdpath == NULL)
	{
		pipex->cmd_ret[1] = 127;
		ft_errhandle(pipex, 1);
	}
	else
	{
		if (execve(cmdpath, pipex->cmd_args[0], pipex->envp) == -1)
		{
			pipex->cmd_ret[1] = 4;
			ft_errhandle(pipex, 1);
		}
	}
	ft_cleanup(pipex);
	exit(1);
}

int	ft_child(t_pipex *pipex, int nb_cmd)
{
	char	*cmdpath;

	dup2(pipex->pipe[(nb_cmd - 1) & 1][0], STDIN_FILENO);
	dup2(pipex->pipe[nb_cmd & 1][1], STDOUT_FILENO);
	ft_close_all_fds(pipex);
	cmdpath = ft_search_cmd(pipex, nb_cmd);
	if (cmdpath == NULL)
	{
		pipex->cmd_ret[nb_cmd] = 127;
		ft_errhandle(pipex, nb_cmd);
	}
	else
	{
		if (execve(cmdpath, pipex->cmd_args[nb_cmd - 1], pipex->envp) == -1)
		{
			pipex->cmd_ret[nb_cmd] = 4;
			ft_errhandle(pipex, nb_cmd);
		}
	}
	ft_cleanup(pipex);
	exit (1);
}

void	ft_cleanup(t_pipex *pipex)
{
	if (pipex->delimiter)
		free(pipex->delimiter);
	if (pipex->infile)
		free(pipex->infile);
	if (pipex->outfile)
		free(pipex->outfile);
	if (pipex->cmds)
		ft_free_2d(pipex->cmds);
	if (pipex->cmd_ret)
		free(pipex->cmd_ret);
	if (pipex->path)
		ft_free_2d(pipex->path);
	if (pipex->cmd_args)
		ft_free_3d(pipex->cmd_args);
}

int	ft_parent_process(t_pipex *pipex)
{
	char	*cmdpath;
	int		fdout;
	char	**cmd_args;
	char	**envp;

	fdout = open(pipex->outfile, O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (fdout < 0)
	{
		ft_printf_err("pipex: %s: %s\n", pipex->outfile, strerror(errno));
		ft_cleanup(pipex);
		exit (1);
	}
	else
	{
		dup2(fdout, STDOUT_FILENO);
		close(fdout);
	}
	dup2(pipex->pipe[(pipex->nb_cmds -1) & 1][0], STDIN_FILENO);
	ft_close_all_fds(pipex);
	cmdpath = ft_search_cmd(pipex, pipex->nb_cmds);
	if (cmdpath == NULL)
	{
		pipex->cmd_ret[pipex->nb_cmds] = 127;
		ft_errhandle(pipex, pipex->nb_cmds);
	}
	else
	{
		cmd_args = pipex->cmd_args[pipex->nb_cmds - 1];
		pipex->cmd_args[pipex->nb_cmds - 1] = NULL;
		envp = pipex->envp;
		if (execve(cmdpath, cmd_args, envp) == -1)
		{
			pipex->cmd_ret[pipex->nb_cmds] = 4;
			ft_errhandle(pipex, pipex->nb_cmds);
		}
	}
	ft_cleanup(pipex);
	exit (1);
}

int	ft_errhandle(t_pipex *pipex, int nb)
{
	if (pipex->cmd_ret[nb] == 13)
		ft_printf_err("pipex: %s: Permission denied\n", pipex->infile);
	if (pipex->cmd_ret[nb] == 2)
		ft_printf_err("pipex: %s: No such file or directory\n", pipex->infile);
	if (pipex->cmd_ret[nb] == 127)
		ft_printf_err("%s: command not found\n", pipex->cmds[nb - 1]);
	if (pipex->cmd_ret[nb] == 4)
		ft_printf_err("pipex: %s: execve error\n", pipex->cmds[nb - 1]);
	return (1);
}

/* 	main function of project pipex.
	Input syntax: ./pipex infile cmd1 ... cmdn outfile
	alternative: ./pipex here_doc LIMITER cmd1 cmd2 outfile
*/
int	main(int argc, char **argv, char **envp)
{
	int		pid_m;
	int		pid_n;
	t_pipex	pipex;
	int		i;

	pid_n = -5;
	ft_init_env(&pipex, &argc, argv, envp);
	if (pipe(pipex.pipe[0]) == -1 || pipe(pipex.pipe[1]) == -1)
	{
		strerror(32);
		return (32);
	}
	ft_validate_args(&pipex);
	ft_parse_cmds(&pipex);
	pipex.path = ft_grab_envp(pipex.envp);
	pid_m = fork();
	if (pid_m < 0)
	{
		strerror(10);
		return (10);
	}
	else if (pid_m == 0)
	{
		// if (pipex.mode == here_doc)
		// 	ft_here_doc(&pipex);
		// else
		ft_first_child(&pipex);
	}
	else
	{
		i = 2;
		while (pid_n != 0 && i < pipex.nb_cmds)
		{
			pid_n = fork();
			if (pid_n == 0)
				ft_child(&pipex, i);
			if (close(pipex.pipe[(i - 1) & 1][0])
				|| close(pipex.pipe[(i - 1) & 1][1]))
				perror("pipex");
			pipe(pipex.pipe[(i - 1) & 1]);
			i++;
		}
		if (pid_n != 0)
		{
			pid_n = fork();
			if (pid_n == 0)
				ft_parent_process(&pipex);
			else
			{
				waitpid(-1, NULL, 0);
				ft_cleanup(&pipex);
			}
		}
	}
}
