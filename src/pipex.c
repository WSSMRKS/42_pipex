/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 13:47:54 by maweiss           #+#    #+#             */
/*   Updated: 2024/06/03 12:09:12 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

void	ft_init_env(t_pipex *pipex, int *argc, char **argv, char **envp)
{
	pipex->argc = *argc;
	pipex->argv = argv;
	pipex->envp = envp;
	pipex->path = NULL;
	pipex->cmds = NULL;
	pipex->cmd_args = NULL;
	pipex->mode = 0;
	pipex->delimiter = NULL;
	pipex->infile = NULL;
	pipex->outfile = NULL;
	pipex->nb_cmds = 0;
}

void	ft_validate_args(t_pipex *pipex)
{
	if (pipex->argc < 5)
	{
		ft_printf_err("Error: Wrong number of arguments\n");
		pipex->mode = error_case;
		exit(1);
	}
	else if (ft_strncmp(pipex->argv[1], "here_doc", 8) == 0)
	{
		if (pipex->argc == 6)
			pipex->mode = here_doc;
		else
		{
			ft_printf_err("Error: Wrong number of arguments for here_doc\n");
			exit(2);
		}
	}
	else if (pipex->argc > 5)
		pipex->mode = bonus_case;
	else
		pipex->mode = base_case;
}

/* Function to destinguish between cases, parse as well as
allocate the commands, args and the in-/outfile */
void	ft_parse_cmds(t_pipex *pipex)
{
	int	nb_cmds;
	int	i;
	int	offset;

	offset = 2;
	nb_cmds = pipex->argc - 3;
	if (pipex->mode == here_doc)
	{
		offset++;
		nb_cmds--;
		pipex->delimiter = ft_strdup(pipex->argv[2]);
	}
	else
		pipex->infile = ft_strdup(pipex->argv[1]);
	i = -1;
	pipex->child_ret = ft_calloc(sizeof(int), nb_cmds - pipex->mode + 1);
	pipex->child_pids = ft_calloc(sizeof(int), nb_cmds - pipex->mode + 1);
	pipex->outfile = ft_strdup(pipex->argv[pipex->argc - 1]);
	pipex->cmd_args = ft_calloc(sizeof(char **), nb_cmds + 1);
	pipex->cmds = ft_calloc(sizeof(char *), nb_cmds + 1);
	while (++i < nb_cmds)
	{
		pipex->cmd_args[i] = ft_split(pipex->argv[i + offset], ' ');
		pipex->cmds[i] = ft_strdup(pipex->cmd_args[i][0]);
	}
	pipex->nb_cmds = nb_cmds;
	pipex->cmd_args[i] = NULL;
	pipex->cmds[i] = NULL;
}

char	**ft_grab_envp(char **envp)
{
	int		i;
	char	**paths;
	char	*tmp;

	i = 0;
	while (envp && envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			break ;
		i++;
	}
	paths = ft_split(&envp[i][5], ':');
	if (paths == NULL)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		if (paths[i][ft_strlen(paths[i]) - 1] != '/')
		{
			tmp = paths[i];
			paths[i] = ft_strjoin(paths[i], "/\0");
			free(tmp);
		}
		i++;
	}
	return (paths);
}

t_list	*ft_pipex_lstnew(void *cont, int type)
{
	t_list	*test;

	test = malloc(sizeof(t_list));
	if (!test)
		return (NULL);
	test->cont = cont;
	test->type = type;
	test->next = NULL;
	return (test);
}

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

void ft_close_all_fds(t_pipex *pipex)
{
	if (close(pipex->pipe[0][0]) || close(pipex->pipe[0][1])
		|| close(pipex->pipe[1][0]) || close(pipex->pipe[1][1]))
		perror("pipex");
}

int	ft_first_child(t_pipex *pipex)
{
	int		fdin;
	char	*cmdpath;
	int		err;

	fdin = open(pipex->infile, O_RDONLY);
	if (fdin < 0)
	{
		ft_cleanup(pipex);
		exit (errno);
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
	free(cmdpath);
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
	exit (err);
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
	if (pipex->child_ret)
		free(pipex->child_ret);
	if (pipex->child_pids)
		free(pipex->child_pids);
	if (pipex->path)
		ft_free_2d(pipex->path);
	if (pipex->cmd_args)
		ft_free_3d(pipex->cmd_args);
}

int	ft_parent_process(t_pipex *pipex)
{
	char	*cmdpath;
	int		fdout;
	int		err;

	fdout = open(pipex->outfile, O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (fdout < 0)
	{
		ft_cleanup(pipex);
		exit (errno);
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
		err = 127;
	else
		err = execve(cmdpath, pipex->cmd_args[pipex->nb_cmds - 1], pipex->envp);
	free(cmdpath);
	ft_cleanup(pipex);
	ft_printf_err("ft_parent_process about to terminate\n");
	exit (err);
}

// int	ft_errhandle(t_pipex *pipex, int nb)
// {
// 	if (pipex->cmd_ret[nb] == 13)
// 		ft_printf_err("pipex: %s: Permission denied\n", pipex->infile);
// 	if (pipex->cmd_ret[nb] == 2)
// 		ft_printf_err("pipex: %s: No such file or directory\n", pipex->infile);
// 	if (pipex->cmd_ret[nb] == 313)
// 		ft_printf_err("%s: command not found\n", pipex->cmds[nb - 1]);
// 	if (pipex->cmd_ret[nb] == 4)
// 		ft_printf_err("pipex: %s: execve error\n", pipex->cmds[nb - 1]);
// 	return (1);
// }

int	ft_wait_error(t_pipex *pipex)
{
	int	i;
	int	err;

	i = 0;
	err = 0;
	while (i < pipex->nb_cmds)
	{
		pipex->child_ret[i] = 0;
		waitpid(pipex->child_pids[i], &(pipex->child_ret[i]), 0);
		ft_printf_err("run number {%d}", i);
		ft_printf_err("exitstatus {%d}", WEXITSTATUS(pipex->child_ret[i]));
		if (WIFEXITED(pipex->child_ret[i]) && WEXITSTATUS(pipex->child_ret[i]) != 0)
		{
			// ft_printf_err("exited, status=%d\n, for child nr %d\n with pid %d\n", WEXITSTATUS(pipex->child_ret[i]), i, pipex->child_pids[i]);
			ft_printf_err("Actual value of pipex->child_ret[i]: {%d} for i nbr {%d}\nError code: %d, strerror: %s\n", pipex->child_ret[i], i, WEXITSTATUS(pipex->child_ret[i]), strerror(WEXITSTATUS(pipex->child_ret[i])));
			// ft_printf_err("exited, status=%d\n, for child nr %d\n", WEXITSTATUS(pipex->child_ret[i]), i);
			if (WEXITSTATUS(pipex->child_ret[i]) == 127)
				ft_printf_err("%s: command not found\n", pipex->cmds[i]);
			else if (WEXITSTATUS(pipex->child_ret[i]) == 13 || WEXITSTATUS(pipex->child_ret[i]) == 2)
			{
				if (i == 0)
					ft_printf_err("pipex: %s: %s\n", pipex->infile, strerror(WEXITSTATUS(pipex->child_ret[i])));
				else
					ft_printf_err("pipex: %s: %s\n", pipex->outfile, strerror(WEXITSTATUS(pipex->child_ret[i])));
			}
			else
				ft_printf_err("%s: %s\n", pipex->cmds[i - 1], strerror(WEXITSTATUS(pipex->child_ret[i])));
			err = 1;
		}
		i++;
	}
	ft_cleanup(pipex);
	exit(err);
}

/* 	main function of project pipex.
	Input syntax: ./pipex infile cmd1 ... cmdn outfile
	alternative: ./pipex here_doc LIMITER cmd1 cmd2 outfile
*/
int	main(int argc, char **argv, char **envp)
{
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
	pipex.child_pids[0] = fork();
	if (pipex.child_pids[0] < 0)
	{
		strerror(10);
		return (10);
	}
	else if (pipex.child_pids[0] == 0)
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
				ft_wait_error(&pipex);
				ft_cleanup(&pipex);
			}
		}
	}
}
