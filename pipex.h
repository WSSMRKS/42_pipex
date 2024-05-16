/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 10:03:45 by maweiss           #+#    #+#             */
/*   Updated: 2024/05/16 12:55:33 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/libft.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct s_pipex {
	char	**path;
	char	**envp;
	char	**argv;
	int		*argc;
	int		fd[2];
}				t_pipex;