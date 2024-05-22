/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_new_approach.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 17:25:29 by maweiss           #+#    #+#             */
/*   Updated: 2024/05/22 18:46:24 by maweiss          ###   ########.fr       */
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

void	ft_init_env(t_pipex *pipex, int *argc, char **argv, char **envp)
{
	pipex->envp = envp;
	pipex->argv = argv;
	pipex->argc = *argc;
	pipex->path = ft_grab_envp(envp);
	pipex->free = ft_pipex_lstnew(NULL, 0);
	pipex->here_doc = 0;
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

void	ft_cmd_first(t_pipex *pipex, int argc_l)
{
	int		i;
	char	*ppath;
	char	**cmd;
	int		infile;
	int		outfile;
	int		sux;
	int		pid_m;

	printf("trying to execute cmd 1\n");
	i = 0;
	sux = -1;
	cmd = ft_split(pipex->argv[2], ' ');
	argc_l = argc_l * 2;
	while (pipex->path[i])
	{
		ppath = ft_strjoin(pipex->path[i], cmd[0]);
		sux = access(ppath, X_OK);
		if (sux == 0)
		{
			break ;
		}
		else
		{
			printf("{%s}\n", ppath);
			free(ppath);
			ppath = NULL;
			i++;
		}
	}
	// if (ft_strncmp(pipex->argv[1], "here_doc", 8 == 0))
	// 	ft_here_doc();
	infile = open("infile", O_RDONLY);
	if (infile < 0)
		perror("");
	dup2(infile, STDIN_FILENO);
	dup2(pipex->pipe1[1], STDOUT_FILENO);
	close(infile);
	close(pipex->pipe1[0]);
	close(pipex->pipe1[1]);
	if (infile > 0)
	{
		pid_m = fork();
		if (pid_m < 0)
		{
			strerror(10);
		}
		else if (pid_m == 0)
		{
			dup2(infile, STDIN_FILENO);
			dup2(pipex->pipe1[1], STDOUT_FILENO);
			close(pipex->pipe1[0]);
			execve(ppath, pipex->argv, pipex->envp);
		}
		else
		{
			outfile = open(pipex->argv[3], O_TRUNC | O_CREAT, 0777);
			dup2(outfile, STDOUT_FILENO);
			dup2(pipex->pipe1[0], STDIN_FILENO);
			wait(NULL);
			close(pipex->pipe1[0]);
			close(pipex->pipe1[1]);
			close(outfile);
		}
	}
}

int	main(int argc, char **argv, char **envp)
{
	int		i;
	// int		pid_m;
	t_pipex	pipex;

	i = 0;
	if (argc < 4)
		ft_printf_err("Error: Wrong number of arguments\n");
	else
	{
		ft_init_env(&pipex, &argc, argv, envp);
		while(pipex.path[i])
		{
			printf("envp Nr. %d:\n%s\n", i, pipex.path[i]);
			i++;
		}
		if (pipe(pipex.pipe1) == -1)
		{
			strerror(32);
			return (32);
		}
		ft_cmd_first(&pipex, 1);
	}
}
