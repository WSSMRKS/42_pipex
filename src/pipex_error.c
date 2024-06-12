/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_error.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/10 23:52:06 by maweiss           #+#    #+#             */
/*   Updated: 2024/06/12 01:58:29 by maweiss          ###   ########.fr       */
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
	ft_printf_err("Entering the exitstatus loop\n");
	while (i < pipex->nb_cmds)
	{
		pipex->child_ret[i] = 0;
		waitpid(pipex->child_pids[i], &(pipex->child_ret[i]), 0);
		if (WIFEXITED(pipex->child_ret[i]))
		{
			if (WEXITSTATUS(pipex->child_ret[i]) == 127 && i != 0)
			{
				ft_printf_err("q: command not found\n", pipex->cmds[i]);
				err = 1;
			}
			if (WEXITSTATUS(pipex->child_ret[i]) == 127
				&& i == pipex->nb_cmds - 1)
			{
				ft_printf_err("%s: command not found\n", pipex->cmds[i]);
				err = 127;
			}
			else if (WEXITSTATUS(pipex->child_ret[i]) == 13
				|| WEXITSTATUS(pipex->child_ret[i]) == 2)
			{
				if (i == 0)
				{
					ft_printf_err("pipex: %s: %s\n", pipex->infile,
						strerror(WEXITSTATUS(pipex->child_ret[i])));
				}
				else
				{
					ft_printf_err("pipex: %s: %s\n", pipex->outfile,
						strerror(WEXITSTATUS(pipex->child_ret[i])));
					err = 1;
				}
			}
			else if (WEXITSTATUS(pipex->child_ret[i]) == 1 && i == 0)
				err = 0;
			else if (WEXITSTATUS(pipex->child_ret[i]) == 1 && i != 0)
				err = 1;
		}
		i++;
	}
	ft_printf_err("err is %d\n", err);
	ft_cleanup(pipex);
	exit(err);
}
