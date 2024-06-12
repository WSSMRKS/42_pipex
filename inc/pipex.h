/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 10:03:45 by maweiss           #+#    #+#             */
/*   Updated: 2024/06/12 12:56:34 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

//current project
# include "../libft/libft.h"
# include <stdio.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <string.h>
# include <errno.h>
# define TEMP_FILE "./.pipex_temp"

//libft
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 100
# endif
# ifndef MAX_FD
#  define MAX_FD 1048
# endif
//current project
// # define USAGE "[infile | here_doc LIMITER] cmd1 cmd2 ... cmdn outfile"
// # define NO_CMD "command not found"
// # define HEREDOC ".temp_pipex_heredoc_file"
// # define MAX_PATH_SIZE		1024
// # define MALLOC_FAIL		42
// # define OPEN_FAIL			2
// # define CLOSE_FAIL			3
// # define FORK_FAIL			4
// # define PIPE_FAIL			5
// # define DUP_FAIL			6
// # define EXECVE_FAIL		127
// # define WRITE_FAIL			8
// # define READ_FAIL			9
// # define TOO_FEW_ARGS		10
// # define WAIT_FAIL			11
// # define ARGS_FAIL			12
// # define CMD_FAIL			-1

typedef enum e_case
{
	error_case,
	regular_case,
	here_doc
}				t_case;

typedef enum e_bool
{
	false,
	true
}				t_bool;

typedef struct s_pipex
{
	int		argc;
	char	**argv;
	char	**envp;
	char	**path;
	char	**cmds;
	int		*cmd_ret;
	int		*child_ret;
	int		*child_pids;
	char	***cmd_args;
	int		pipe[2][2];
	int		mode;
	char	*delimiter;
	char	*infile;
	char	*outfile;
	int		nb_cmds;
}				t_pipex;

// pipex
void	ft_init_env(t_pipex *pipex, int *argc, char **argv, char **envp);
void	ft_validate_args(t_pipex *pipex);
int		ft_parse_cmds(t_pipex *pipex);
char	**ft_grab_envp(char **envp);
char	*ft_search_cmd(t_pipex *pipex, int nbcmd);
int		ft_first_child(t_pipex *pipex);
int		ft_parent_process(t_pipex *pipex);
void	ft_cleanup(t_pipex *pipex);
int		ft_child(t_pipex *pipex, int nb_cmd);
int		ft_errhandle(t_pipex *pipex, int nb);
void	ft_close_all_fds(t_pipex *pipex);
int		ft_wait_error(t_pipex *pipex);

#endif
