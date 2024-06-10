/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_error.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/10 23:52:06 by maweiss           #+#    #+#             */
/*   Updated: 2024/06/11 01:39:29 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

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