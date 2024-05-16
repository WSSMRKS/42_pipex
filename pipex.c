/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 09:54:20 by maweiss           #+#    #+#             */
/*   Updated: 2024/05/16 12:56:00 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int ft_final_child(t_pipex *pipex)
{
	int	pid;
	
	pid = fork();
	if (pid < 0)
	{
		strerror(10);
		exit(10);
	}
	else if (pid == 0)
	{
		pipex->argc = pipex->argc - 2;
		ft_child_n(pipex);
	}
	else
	{
		waitpid(pid, NULL, 0);
		//execute last command, write to file;
		
	}
	if (ft_strncmp(pipex->argv[1], "here_doc", 8 == 0))
		ft_here_doc();
	
}

char **ft_grab_envp(char **envp)
{
	int	i;

	i = 0;
	while(envp && envp[i])
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
}


int	main(int argc, char **argv, char **envp)
{
	int		i;
	int		pid1;
	t_pipex	pipex;
	

	i = 0;
	if (argc < 5)
		ft_printf_err("Error: Wrong number of arguments");
	else
	{
		// regular execution
		// create child process. (last 2 args get first (flip the line.)
		// incorporate PATH
		ft_init_env(pipex, &argc, argv, envp);
		if (pipe(pipex.fd) == -1)
		{
			strerror(32);
			return (32);
		}
		pid1 = fork();
		if (pid1 < 0)
		{
			strerror(10);
			return (10);
		}
		else if (pid1 == 0)
			ft_final_child(&pipex);
		else
			waitpid(pid1, NULL, 0);
	}		
}