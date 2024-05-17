/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execve.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 12:56:41 by maweiss           #+#    #+#             */
/*   Updated: 2024/05/17 11:22:29 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 08:07:31 by maweiss           #+#    #+#             */
/*   Updated: 2024/05/14 09:35:48 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int	main(int argc, char **argv)
{
	int	fd[2];
	int	pid1;
	int	pid2;
	int	calc1;
	int	calc2;
	int	part_sum1;
	int	part_sum2;
	int	sum;

	calc1 = 0;
	calc2 = 0;
	part_sum1 = 0;
	part_sum2 = 0;
	sum = 0;

	if (pipe(fd) == -1)
		return (1);
	pid1 = fork();
	if (pid1 < 0)
		return (2);
	else if (pid1 == 0)
	{
		// Child process1;
		printf("I am child 1\n");
		pid2 = fork();
		if (pid2 < 0)
			return (3);
		else if (pid2 == 0)
		{
			// Child process2;

			write(fd[1], &calc2, sizeof(calc2));
			close(fd[0]);
			close(fd[1]);
			printf("I am child 2\n");
		}
		else
		{
			printf("I am child 1 again\n");
			calc1 = 20000 * 3 / 2;
			write(fd[1], &calc1, sizeof(calc1));
			printf("waiting for child process 2\n");
			wait(NULL);
			close(fd[0]);
			close(fd[1]);
		}
	}
	else
	{
		wait(NULL);
		read(fd[0], &part_sum1, sizeof(part_sum2));
		read(fd[0], &part_sum2, sizeof(part_sum2));
		close(fd[0]);
		close(fd[1]);
		sum = part_sum1 + part_sum2;
		printf("the sums are:\npart_sum1: %d\npart_sum2: %d\nsum: %d\n",
			part_sum1, part_sum2, sum);
		return (0);
	}
}
