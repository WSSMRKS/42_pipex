/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 13:47:54 by maweiss           #+#    #+#             */
/*   Updated: 2024/05/30 18:04:52 by maweiss          ###   ########.fr       */
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
	pipex->pipe1[0] = 0;
	pipex->pipe1[1] = 0;
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

/* Function to destinguish between cases and parse + allocate the commands, args and the in-/outfile */
void	ft_parse_cmds(t_pipex *pipex)
{
	int	nb_cmds;
	int i;
	int offset;

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
	pipex->cmds = ft_calloc(sizeof(char *), nb_cmds + 1); // [ ] free
	while (++i < nb_cmds)
	{
		pipex->cmd_args[i] = ft_split(pipex->argv[i + offset], ' ');
		pipex->cmds[i] = ft_strdup(pipex->cmd_args[i][0]); // [ ] free
    	printf("Command %d: %s\n", i, pipex->cmds[i]); // Debug print
   		for (int j = 0; pipex->cmd_args[i][j]; j++) {
        printf("Arg %d: %s\n", j, pipex->cmd_args[i][j]); // Debug print
		}
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

char *ft_search_cmd(t_pipex *pipex, int nbcmd)
{
	int		i;
	char	*path;
	
	i = 0;
	while(pipex->path[i])
	{
		path = ft_strjoin(pipex->path[i], pipex->cmds[nbcmd - 1]);
		if (path == NULL)
		{
			perror("malloc fail!\n");
			exit(4);
		}
		if (access(path, X_OK) == 0)
			break ;
		free(path);
		i++;
	}
	return (path);
}

int	ft_first_child(t_pipex *pipex)
{
	int		fdin;
	char	*cmdpath;
	
	close(pipex->pipe1[0]);
	fdin = open(pipex->infile, O_RDONLY);
	if (fdin < 0)
	{
		perror("infile");
		return (2);
	}
	else
	{
		dup2(fdin, STDIN_FILENO);
		close(fdin);
	}
	dup2(pipex->pipe1[1], STDOUT_FILENO);
	close(pipex->pipe1[1]);
	cmdpath = ft_search_cmd(pipex, 1);
	if (cmdpath == NULL)
		perror("command not found");
	else
	{
		ft_printf_err("cmdpath child: {%s}\n", cmdpath);
		if (execve(cmdpath, pipex->cmd_args[0], pipex->envp) == -1)
		{
			free(cmdpath);
			perror("Execve failed!\n");
		}
	}
	return (1);
}

int	ft_child(t_pipex *pipex, int nb_cmd)
{
	char	*cmdpath;

	dup2(pipex->pipe1[0], STDIN_FILENO);
	dup2(stdout, STDOUT_FILENO);
	close(pipex->pipe1[0]);
	close(pipex->pipe1[1]);
	cmdpath = ft_search_cmd(pipex, nb_cmd);
	if (cmdpath == NULL)
		perror("command not found");
	else
	{
		ft_printf_err("cmdpath child: {%s}\n", cmdpath);
		if (execve(cmdpath, pipex->cmd_args[nb_cmd - 1], pipex->envp) == -1)
		{
			free(cmdpath);
			perror("Execve failed!\n");
		}
	}
	return (1);
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

	close(pipex->pipe1[1]);
	fdout = open(pipex->outfile, O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (fdout < 0)
	{
		perror("outfile");
		return (2);
	}
	else
	{
		dup2(fdout, STDOUT_FILENO);
		close(fdout);
	}
	dup2(pipex->pipe1[0], STDIN_FILENO);
	close(pipex->pipe1[0]);
	cmdpath = ft_search_cmd(pipex, 2);
	if (cmdpath == NULL)
		perror("command not found");
	else
	{
		ft_printf_err("cmdpath parent: {%s}\n", cmdpath);
		cmd_args = pipex->cmd_args[pipex->nb_cmds - 1];
		pipex->cmd_args[pipex->nb_cmds - 1] = NULL;
		envp = pipex->envp;
		ft_cleanup(pipex);
		if (execve(cmdpath, cmd_args, envp) == -1)
		{
			free(cmdpath);
			perror("execve failed!\n");
		}
	}
	return (1);
}

/* 	main function of project pipex. Input syntax: ./pipex infile cmd1 ... cmdn outfile 
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
	ft_validate_args(&pipex);
	ft_parse_cmds(&pipex);
	pipex.path = ft_grab_envp(pipex.envp);
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
		// if (pipex.mode == here_doc)
		// 	ft_here_doc(&pipex);
		// else
			ft_first_child(&pipex);
	else
	{
		i = 2;
		while (pid_n != 0 && i < pipex.nb_cmds)
		{
			pid_n = fork();
			if (pid_n == 0)
				ft_child(&pipex, i); //execute command 2, 3, n;
			i++;
		}
		if (pid_n != 0)
		{
			waitpid(-1, NULL, 0);
			ft_parent_process(&pipex);
		}
	}
}
