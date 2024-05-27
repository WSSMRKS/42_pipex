/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   06_multiprocessing_using_pipes.c                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 10:21:31 by maweiss           #+#    #+#             */
/*   Updated: 2024/05/27 10:57:46 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <../libft/libft.h>

int	main(int argc, char **argv, char **envp)
{
	int		fds[2];
	char	*res;
	int		id1;
	int		id2;

	if (pipe(fds) == -1)
	{
		printf("error when initializing pipe\n");
		return (1);
	}
	id1 = fork();
	if (id1 == 0)
	{
		char	*string1;
		int		size1;

		printf("we are in the first child process with id: %d\n", id1);
		close(fds[0]);
		printf("Input a string to pass to the parent: ");
		scanf("%s", string1);
		size1 = ft_strlen(string1);
		write(fds[1], &size, sizeof(int));
		write(fds[1], &string1, sizeof(string1));
		close(fds[1]);
	}
	else
	{
		id2 = fork();
		if (id2 == 0)
		{
			char	*string2;
			int		size2;

			printf("we are in the second child process with id: %d\n", id2);
			close(fds[0]);
			printf("Input a string to pass to the parent: ");
			scanf("%s", string2);
			size2 = ft_strlen(string2);
			write(fds[1], &size2, sizeof(int));
			write(fds[1], &string2, sizeof(string2));
			close(fds[1]);
		}
		else
		{
			char	*res1;
			int		res1_s;
			char	*res2;
			int		res2_s;
			char	*res;

			close(fds[1]);
			printf("we are in the parent process with id1: %d and id2: %d\n", id1, id2);
			read(fds[0], &res1_s, sizeof(int));
			res1 = ft_calloc(sizeof(char), res1_s + 1);
			read(fds[0], &res1, (res1_s + 1));
			printf("I received the following string from child1: {%s}\n", res1);
			read(fds[0], res2_s, sizeof(int));
			res2 = ft_calloc(sizeof(char), res2_s + 1);
			read(fds[0], &res1, (res2_s + 1));
			printf("I received the following string from child2: {%s}\n", res1);
			res = ft_strjoin(res1, res2);
			free(res1);
			free(res2);
			printf("the full string received from the childs is: {%s}\n", res);
			close(fds[0]);
		}
	}
}
