/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_error.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/10 23:52:06 by maweiss           #+#    #+#             */
/*   Updated: 2024/06/10 23:52:26 by maweiss          ###   ########.fr       */
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