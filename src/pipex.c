/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 09:54:20 by maweiss           #+#    #+#             */
/*   Updated: 2024/05/22 16:13:22 by maweiss          ###   ########.fr       */
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

void	ft_cmd_first(t_pipex *pipex, int argc_l)
{
	int		i;
	char	*ppath;
	char	**cmd;
	int		infile;
	int		sux;

	printf("trying to execute cmd 1\n");
	i = 0;
	sux = -1;
	cmd = ft_split(pipex->argv[argc_l - 1], ' ');
	ft_lstadd_back(&pipex->free, ft_pipex_lstnew(cmd, 2));
	while (pipex->path[i])
	{
		ppath = ft_strjoin(pipex->path[i], cmd[0]);
		sux = access(ppath, X_OK);
		if (sux == 0)
		{
			printf("Path to cmd 1 found.\nPath is:\n");
			printf("%s", ppath);
			ft_printf("%s", ppath);
			break ;
		}
		else
		{
			printf("{%s}\n", ppath);
			// free(ppath);
			// ppath = NULL;
			i++;
		}
	}
	// if (ft_strncmp(pipex->argv[1], "here_doc", 8 == 0))
	// 	ft_here_doc();
	infile = open(pipex->argv[1], O_RDONLY);
	if (infile < 0)
		perror("pipex");
	dup2(infile, STDIN_FILENO);
	dup2(pipex->pipe1[1], STDOUT_FILENO);
	close(infile);
	close(pipex->pipe1[0]);
	close(pipex->pipe1[1]);
	if (infile > 0)
		execve(ppath, pipex->argv, pipex->envp);
}

void	ft_child(t_pipex *pipex)
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

void	ft_cmd_n(t_pipex *pipex, int argc_l)
{
	int		i;
	char	*ppath;
	char	**cmd;

	i = 0;
	cmd = ft_split(pipex->argv[argc_l - 3], ' ');
	ft_lstadd_back(&pipex->free, ft_pipex_lstnew(cmd, 2));
	while (pipex->path[i])
	{
		ppath = ft_strjoin(pipex->path[i], cmd[0]);
		if (access(ppath, X_OK) == 0)
			break ;
		free(ppath);
		ppath = NULL;
		i++;
	}
	dup2(pipex->pipe1[1], STDOUT_FILENO);
	dup2(pipex->pipe1[0], STDIN_FILENO);
	close(pipex->pipe1[0]);
	close(pipex->pipe1[1]);
	if (ppath == NULL)
		ft_lstadd_back(&pipex->free, ft_pipex_lstnew(ppath, 1));
	execve(ppath, &pipex->argv[argc_l - 3], pipex->envp);
	strerror(2);
}

void	ft_parent_process(t_pipex *pipex)
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

void	ft_cmd_last(t_pipex *pipex, int argc_l)
{
	int		i;
	char	*ppath;
	char	**cmd;
	int		outfile;
	int		sux;

	printf("we are here3\n");
	i = 0;
	sux = -1;
	cmd = ft_split(pipex->argv[argc_l - 2], ' ');
	ft_lstadd_back(&pipex->free, ft_pipex_lstnew(cmd, 2));
	while (pipex->path[i])
	{
		ppath = ft_strjoin(pipex->path[i], cmd[0]);
		sux = access(ppath, X_OK);
		if (sux == 0)
			break ;
		free(ppath);
		ppath = NULL;
		i++;
	}
	printf("we are here4\n");
	outfile = open(pipex->argv[argc_l - 1], O_TRUNC | O_CREAT, 0777);
	printf("we are here4,5\n");
	dup2(outfile, STDOUT_FILENO);
	dup2(pipex->pipe1[0], STDIN_FILENO);
	printf("we are here5\n");
	close(outfile);
	close(pipex->pipe1[0]);
	close(pipex->pipe1[1]);
	printf("we are here6\n");
	execve(ppath, &pipex->argv[argc_l - 1], pipex->envp);
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

void	ft_init_env(t_pipex *pipex, int *argc, char **argv, char **envp)
{
	pipex->envp = envp;
	pipex->argv = argv;
	pipex->argc = *argc;
	pipex->path = ft_grab_envp(envp);
	pipex->free = ft_pipex_lstnew(NULL, 0);
	pipex->here_doc = 0;
}

int	main(int argc, char **argv, char **envp)
{
	int		i;
	int		pid_m;
	t_pipex	pipex;

	i = 0;
	if (argc < 5)
		ft_printf_err("Error: Wrong number of arguments\n");
	else
	{
		ft_init_env(&pipex, &argc, argv, envp);
		if (ft_strncmp(argv[1], "here_doc", 8) == 0)
			pipex.here_doc = 1;
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
		{
			waitpid(pid_m, NULL, 0);
			close(pipex.pipe1[0]);
			close(pipex.pipe1[1]);
		}
	}
}
