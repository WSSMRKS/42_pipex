/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 13:47:54 by maweiss           #+#    #+#             */
/*   Updated: 2024/06/14 20:26:16 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

/* 	main function of project pipex.
	Input syntax: ./pipex infile cmd1 ... cmdn outfile
	alternative: ./pipex here_doc LIMITER cmd1 cmd2 outfile
*/
int	main(int argc, char **argv, char **envp)
{
	t_pipex	pipex;
	int		i;

	ft_init_env(&pipex, &argc, argv, envp);
	ft_pipex_init(&pipex);
	i = 0;
	ft_fprintf(2, "nb_cmds == %d\n", pipex.nb_cmds);
	ft_fork_first_child(&pipex, &i);
	if (pipex.nb_cmds > 2)
		ft_malcolm(&pipex, &i);
	else
		i++;
	pipex.child_pids[i] = fork();
	if (pipex.child_pids[i] == 0)
		ft_parent_process(&pipex);
	ft_close_all_fds(&pipex);
	if (pipex.mode == here_doc)
	{
		unlink(TEMP_FILE);
		if (!access(TEMP_FILE, F_OK))
			ft_printf_err("pipex: could not delete tempfile\n");
	}
	ft_wait_error(&pipex);
}
