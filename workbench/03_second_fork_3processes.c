/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   03_second_fork_3processes.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 15:12:29 by maweiss           #+#    #+#             */
/*   Updated: 2024/05/25 15:25:17 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>

/*start a new process if still in main results in 3 processes.*/
int main(int argc, char **argv)
{
	int id;
	int id2;
	
	id2 = -1;
	id = fork();
	if (id != 0)
		id2 = fork();
	printf("Hello world from process with id1: %d id2: %d\n", id, id2);
	return (0);
}