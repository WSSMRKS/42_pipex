/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_parsing.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/10 23:21:53 by maweiss           #+#    #+#             */
/*   Updated: 2024/06/11 01:36:21 by maweiss          ###   ########.fr       */
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

int	ft_alloc_t_pipex(t_pipex *pipex)
{
	int	ret;

	ret = 0;
	pipex->cmd_ret = ft_calloc(sizeof(int), pipex->nb_cmds + 2);
	if (pipex->cmd_ret == NULL)
		ret = -1;
	pipex->child_ret = ft_calloc(sizeof(int), pipex->nb_cmds - pipex->mode + 1);
	if (pipex->child_ret == NULL)
		ret = -1;
	pipex->child_pids = ft_calloc(sizeof(int), pipex->nb_cmds - pipex->mode + 1);
	if (pipex->child_pids == NULL)
		ret = -1;
	pipex->outfile = ft_strdup(pipex->argv[pipex->argc - 1]);
	if (pipex->outfile == NULL)
		ret = -1;
	pipex->cmd_args = ft_calloc(sizeof(char **), pipex->nb_cmds + 1);
	if (pipex->cmd_args == NULL)
		ret = -1;
	pipex->cmds = ft_calloc(sizeof(char *), pipex->nb_cmds + 1);
	if (pipex->cmds == NULL)
		ret = -1;
	return (ret);
	}

/* Function to destinguish between cases, parse as well as
allocate the commands, args and the in-/outfile */
int	ft_parse_cmds(t_pipex *pipex)
{
	int	i;
	int	offset;

	offset = 2;
	pipex->nb_cmds = pipex->argc - 3;
	if (pipex->mode == here_doc)
	{
		offset++;
		pipex->nb_cmds -= 1;
		pipex->delimiter = ft_strdup(pipex->argv[2]);
	}
	else
		pipex->infile = ft_strdup(pipex->argv[1]);
	i = -1;
	if (ft_alloc_t_pipex(pipex) == -1)
		return(-1);
	while (++i < pipex->nb_cmds)
	{
		pipex->cmd_args[i] = ft_split(pipex->argv[i + offset], ' ');
		pipex->cmds[i] = ft_strdup(pipex->cmd_args[i][0]);
	}
	pipex->cmd_args[i] = NULL;
	pipex->cmds[i] = NULL;
	return (0);
}

char	*ft_search_cmd(t_pipex *pipex, int nbcmd)
{
	int		i;
	char	*path;
	int		sux;

	i = 0;
	sux = 0;
	while (pipex->path[i])
	{
		path = ft_strjoin(pipex->path[i], pipex->cmds[nbcmd - 1]);
		if (path == NULL)
		{
			perror("malloc fail!\n");
			exit(4);
		}
		if (access(path, X_OK) == 0)
			return (path);
		free(path);
		i++;
	}
	return (NULL);
}