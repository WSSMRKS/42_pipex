/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 09:54:20 by maweiss           #+#    #+#             */
/*   Updated: 2024/05/17 13:04:23 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

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

int	ft_cmd_first(t_pipex *pipex, int argc_l)
{
	int		i;
	char	*ppath;
	char	**cmd;
	int		infile;

	i = 0;
	cmd = ft_split(pipex->argv[*pipex->argc], ' ');
	ft_lstadd_back(pipex->free, ft_pipex_lstnew(cmd, 2));
	while (pipex->path[i])
	{
		ppath = ft_strjoin(pipex->path[i], cmd[0]);
		if (access(ppath, X_OK) == 0)
			break ;
		free(ppath);
		i++;
	}
	ft_lstadd_back(pipex->free, ft_pipex_lstnew(ppath, 1));
	if (ft_strncmp(pipex->argv[1], "here_doc", 8 == 0))
		ft_here_doc();
	else
	{
		infile = open(pipex->argv[1], O_RDONLY);
		dup2(infile, STDIN_FILENO);
		dup2(pipex->pipe1[1], STDOUT_FILENO);
		close(infile);
		close(pipex->pipe1[0]);
		close(pipex->pipe1[1]);
		execve(ppath, &cmd[1], pipex->envp);
	}
}

int	ft_child(t_pipex *pipex)
{
	int	pid_n;
	int	argc_l;

	argc_l = pipex->argc;
	if (argc_l <= 3)
		ft_cmd_first(pipex, argc_l);
	else
	{
		pid_n = fork();
		if (pid_n < 0)
		{
			strerror(10);
			exit(10);
		}
		else if (pid_n == 0)
		{
			pipex->argc = pipex->argc - 1;
			ft_child(pipex);
		}
		else
		{
			waitpid(pid_n, NULL, 0);
			ft_cmd_n(pipex, argc_l);
		}
	}
}

int	ft_cmd_n(t_pipex *pipex, int argc_l)
{
	int		i;
	char	*ppath;
	char	**cmd;

	i = 0;
	cmd = ft_split(pipex->argv[argc_l - 3], ' ');
	ft_lstadd_back(pipex->free, ft_pipex_lstnew(cmd, 2));
	while (pipex->path[i])
	{
		ppath = ft_strjoin(pipex->path[i], cmd[0]);
		if (access(ppath, X_OK) == 0)
			break ;
		free(ppath);
		i++;
	}
	ft_lstadd_back(pipex->free, ft_pipex_lstnew(ppath, 1));
	dup2(pipex->pipe1[1], STDOUT_FILENO);
	dup2(pipex->pipe1[0], STDIN_FILENO);
	close(pipex->pipe1[0]);
	close(pipex->pipe1[1]);
	execve(ppath, &cmd[1], pipex->envp);
}

int	ft_parent_process(t_pipex *pipex)
{
	int	pid_f;
	int	argc_l;

	argc_l = pipex->argc;
	pid_f = fork();
	if (pid_f < 0)
	{
		strerror(10);
		exit(10);
	}
	else if (pid_f == 0)
	{
		pipex->argc = pipex->argc - 2;
		ft_child(pipex);
	}
	else
	{
		waitpid(pid_f, NULL, 0);
		ft_cmd_last(pipex, argc_l);
	}
}

int	ft_cmd_last(t_pipex *pipex, int argc_l)
{
	int		i;
	char	*ppath;
	char	**cmd;
	int		outfile;

	i = 0;
	cmd = ft_split(pipex->argv[argc_l - 2], ' ');
	ft_lstadd_back(pipex->free, ft_pipex_lstnew(cmd, 2));
	while (pipex->path[i])
	{
		ppath = ft_strjoin(pipex->path[i], cmd[0]);
		if (access(ppath, X_OK) == 0)
			break ;
		free(ppath);
		i++;
	}
	ft_lstadd_back(pipex->free, ft_pipex_lstnew(ppath, 1));
	if (pipex->here_doc == 1)
		outfile = open(pipex->argv[argc_l - 1], O_APPEND | O_CREAT, 0777);
	else
		outfile = open(pipex->argv[1], O_TRUNC | O_CREAT, 0777);
	dup2(outfile, STDOUT_FILENO);
	dup2(pipex->pipe1[0], STDIN_FILENO);
	close(outfile);
	close(pipex->pipe1[0]);
	execve(ppath, &cmd[1], pipex->envp);
}

char	**ft_grab_envp(char **envp)
{
	int	i;

	i = 0;
	while (envp && envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			break ;
		i++;
	}
	return (ft_split(envp[i], ':'));
}

void	ft_init_env(t_pipex pipex, int *argc, char **argv, char **envp)
{
	pipex.envp = envp;
	pipex.argv = argv;
	pipex.argc = argc;
	pipex.path = ft_grab_envp(envp);
	pipex.free = ft_pipex_lstnew(NULL, 0);
}

int	main(int argc, char **argv, char **envp)
{
	int		i;
	int		pid_m;
	t_pipex	pipex;

	i = 0;
	if (argc < 5)
		ft_printf_err("Error: Wrong number of arguments");
	else
	{
		if (ft_strncmp(argv[1], "here_doc", 8) == 0)
			pipex.here_doc = 1;
		ft_init_env(pipex, &argc, argv, envp);
		if (pipe(pipex.pipe0) == -1)
		{
			strerror(32);
			return (32);
		}
		if (pipe(pipex.pipe1) == -1)
		{
			strerror(32);
			return (32);
		}
		pid_m = fork();
		if (pid_m < 0)
		{
			strerror(10);
			return (10);
		}
		else if (pid_m == 0)
			ft_parent_process(&pipex);
		else
			waitpid(pid_m, NULL, 0);
	}
}
