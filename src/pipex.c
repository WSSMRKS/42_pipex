/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 13:47:54 by maweiss           #+#    #+#             */
/*   Updated: 2024/05/28 17:54:49 by maweiss          ###   ########.fr       */
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

// void	ft_cmd_first(t_pipex *pipex, int argc_l)
// {
// 	int		i;
// 	char	*ppath;
// 	char	**cmd;
// 	int		infile;
// 	int		sux;

// 	printf("trying to execute cmd 1\n");
// 	i = 0;
// 	sux = -1;
// 	cmd = ft_split(pipex->argv[argc_l - 1], ' ');
// 	ft_lstadd_back(&pipex->free, ft_pipex_lstnew(cmd, 2));
// 	while (pipex->path[i])
// 	{
// 		ppath = ft_strjoin(pipex->path[i], cmd[0]);
// 		sux = access(ppath, X_OK);
// 		if (sux == 0)
// 		{
// 			printf("Path to cmd 1 found.\nPath is:\n");
// 			printf("printf %s\n", ppath);
// 			ft_printf("ft_printf %s\n", ppath);
// 			break ;
// 		}
// 		else
// 		{
// 			printf("{%s}\n", ppath);
// 			// free(ppath);
// 			// ppath = NULL;
// 			i++;
// 		}
// 	}
// 	// if (ft_strncmp(pipex->argv[1], "here_doc", 8 == 0))
// 	// 	ft_here_doc();
// 	infile = open(pipex->argv[1], O_RDONLY);
// 	if (infile < 0)
// 		perror("pipex");
// 	dup2(infile, STDIN_FILENO);
// 	dup2(pipex->pipe1[1], STDOUT_FILENO);
// 	close(infile);
// 	close(pipex->pipe1[0]);
// 	close(pipex->pipe1[1]);
// 	if (infile > 0)
// 		execve(ppath, pipex->argv, pipex->envp);
// }

// void	ft_cmd_n(t_pipex *pipex, int argc_l)
// {
// 	int		i;
// 	char	*ppath;
// 	char	**cmd;

// 	i = 0;
// 	cmd = ft_split(pipex->argv[argc_l - 3], ' ');
// 	ft_lstadd_back(&pipex->free, ft_pipex_lstnew(cmd, 2));
// 	while (pipex->path[i])
// 	{
// 		ppath = ft_strjoin(pipex->path[i], cmd[0]);
// 		if (access(ppath, X_OK) == 0)
// 			break ;
// 		free(ppath);
// 		ppath = NULL;
// 		i++;
// 	}
// 	dup2(pipex->pipe1[1], STDOUT_FILENO);
// 	dup2(pipex->pipe1[0], STDIN_FILENO);
// 	close(pipex->pipe1[0]);
// 	close(pipex->pipe1[1]);
// 	if (ppath == NULL)
// 		ft_lstadd_back(&pipex->free, ft_pipex_lstnew(ppath, 1));
// 	execve(ppath, &pipex->argv[argc_l - 3], pipex->envp);
// 	strerror(2);
// }

// void	ft_cmd_last(t_pipex *pipex, int argc_l)
// {
// 	int		i;
// 	char	*ppath;
// 	char	**cmd;
// 	int		outfile;
// 	int		sux;

// 	printf("we are here3\n");
// 	i = 0;
// 	sux = -1;
// 	cmd = ft_split(pipex->argv[argc_l - 2], ' ');
// 	ft_lstadd_back(&pipex->free, ft_pipex_lstnew(cmd, 2));
// 	while (pipex->path[i])
// 	{
// 		ppath = ft_strjoin(pipex->path[i], cmd[0]);
// 		sux = access(ppath, X_OK);
// 		if (sux == 0)
// 			break ;
// 		free(ppath);
// 		ppath = NULL;
// 		i++;
// 	}
// 	printf("we are here4\n");
// 	outfile = open(pipex->argv[argc_l - 1], O_TRUNC | O_CREAT, 0777);
// 	printf("we are here4,5\n");
// 	dup2(outfile, STDOUT_FILENO);
// 	dup2(pipex->pipe1[0], STDIN_FILENO);
// 	printf("we are here5\n");
// 	close(outfile);
// 	close(pipex->pipe1[0]);
// 	close(pipex->pipe1[1]);
// 	printf("we are here6\n");
// 	execve(ppath, &pipex->argv[argc_l - 1], pipex->envp);
// }

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
	pipex->argc = *argc;
	pipex->argv = argv;
	pipex->envp = envp;
	pipex->path = NULL;
	pipex->cmds = NULL;
	pipex->cmd_args = NULL;
	pipex->pipe1[0] = 0;
	pipex->pipe1[1] = 0;
	pipex->free = NULL;
	
}

// void	ft_child(t_pipex *pipex)
// {
// 	int	pid_n;
// 	int	argc_l;

// 	argc_l = pipex->argc;
// 	if (argc_l == 5)
// 		ft_cmd_first(pipex, argc_l);
// 	else
// 	{
// 		while(pid_n != 0)
// 			pid_n = fork();
// 		if (pid_n < 0)
// 		{
// 			strerror(10);
// 			exit(10);
// 		}
// 		else if (pid_n == 0)
// 		{
// 			pipex->argc = pipex->argc - 1;
// 			ft_child(pipex);
// 		}
// 		else
// 		{
// 			waitpid(pid_n, NULL, 0);
// 			ft_cmd_n(pipex, argc_l);
// 		}
// 	}
// }

// void	ft_parent_process(t_pipex *pipex)
// {
// 	ft_cmd_last(pipex);
// }

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

/* Function to destinguish between cases and parse + allocate the commands, args and the in-/outfile */
void	ft_parse_cmds(t_pipex *pipex)
{
	int	nb_cmds;
	int i;
	int offset;

	nb_cmds = 0;
	offset = 2;
	nb_cmds = pipex->argc - 3;
	
	if (pipex->mode == here_doc)
	{
		offset++;
		nb_cmds--;
		pipex->delimiter = ft_strdup(pipex->argv[2]); //[ ] free
	}
	else
		pipex->infile = ft_strdup(pipex->argv[1]); // [ ] free
	i = -1;
	pipex->outfile = ft_strdup(pipex->argv[pipex->argc - 1]); // [ ] free
	pipex->cmd_args = ft_calloc(sizeof(char **), nb_cmds + 1); // [ ] free
	pipex->cmds = ft_calloc(sizeof(char *), nb_cmds + 1);
	while (++i < nb_cmds)
	{
		pipex->cmd_args[i] = ft_split(pipex->argv[i + offset], ' ');
		pipex->cmds[i] = ft_strdup(pipex->cmd_args[i][0]); // [ ] free
	}
	pipex->cmd_args[i] = NULL;
	pipex->cmds[i] = NULL;
}

/*
main()
{
	ft_init_pipex()
	ft_check_args()
	ft_parse_cmds()
	ft_parse_args()
	while (cmds)
		ft_exec()
	ft_cleanup()
}
*/
int	main(int argc, char **argv, char **envp)
{
	// int		pid_m;
	// int		pid_n;
	t_pipex	pipex;

	ft_init_env(&pipex, &argc, argv, envp);
	ft_validate_args(&pipex);
	ft_parse_cmds(&pipex);
	pipex.path = ft_grab_envp(pipex.envp);
	// if (pipe(pipex.pipe1) == -1)
	// 	{
	// 		strerror(32);
	// 		return (32);
	// 	}
	// 	while (argc > 5)
		
	// 	pid_m = fork();
	// 	if (pid_m < 0)
	// 	{
	// 		strerror(10);
	// 		return (10);
	// 	}
	// 	else if (pid_m == 0)
	// 		ft_first_child(&pipex);
	// 	else
	// 	{
	// 		// while (argc > 5)
	// 		// {
	// 		// 	pid_n = fork();
	// 		// 	if (pid_n == 0)
	// 		// 		ft_child(&pipex) //execute command 2, 3, n;			
	// 		// }
	// 		if (pid_n != 0)
	// 			ft_parent_process(&pipex);
	// 		// close(pipex.pipe1[1]);
	// 		// waitpid(-1, NULL, 0);
	// 		// // open file
	// 		// // dup 2 stdout to outfile
	// 		// // execve command last
	// 		// close(pipex.pipe1[0]);
	// 	}
	// }
}