/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_parsing.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/10 23:21:53 by maweiss           #+#    #+#             */
/*   Updated: 2024/06/10 23:22:03 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

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

/* Function to destinguish between cases, parse as well as
allocate the commands, args and the in-/outfile */
void	ft_parse_cmds(t_pipex *pipex)
{
	int	nb_cmds;
	int	i;
	int	offset;

	offset = 2;
	nb_cmds = pipex->argc - 3;
	if (pipex->mode == here_doc)
	{
		offset++;
		nb_cmds--;
		pipex->delimiter = ft_strdup(pipex->argv[2]);
	}
	else
		pipex->infile = ft_strdup(pipex->argv[1]);
	i = -1;
	pipex->cmd_ret = ft_calloc(sizeof(int), nb_cmds + 2);
	pipex->outfile = ft_strdup(pipex->argv[pipex->argc - 1]);
	pipex->cmd_args = ft_calloc(sizeof(char **), nb_cmds + 1);
	pipex->cmds = ft_calloc(sizeof(char *), nb_cmds + 1);
	while (++i < nb_cmds)
	{
		pipex->cmd_args[i] = ft_split(pipex->argv[i + offset], ' ');
		pipex->cmds[i] = ft_strdup(pipex->cmd_args[i][0]);
	}
	pipex->nb_cmds = nb_cmds;
	pipex->cmd_args[i] = NULL;
	pipex->cmds[i] = NULL;
}