/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   04_forking&printing.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 15:12:29 by maweiss           #+#    #+#             */
/*   Updated: 2024/05/25 16:03:24 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/wait.h>

/*Print numbers in two processes. Use wait to guarantee the numbers are printed in order. Using fflush or not makes a huge difference. */
int main(int argc, char **argv)
{
	int id;
	int n;
	int i;
	
	id = fork();
	if (id == 0)
		n = 1;
	else
		n = 6;
	i = n;
	if (id != 0)
		wait(NULL);
	while (i < n + 5)
	{
		printf("%d ", i);
		fflush(stdout);
		i++;
	}
	if (id != 0)
		printf("\n");
	return (0);
}