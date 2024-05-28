/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   02_second_fork.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 15:12:29 by maweiss           #+#    #+#             */
/*   Updated: 2024/05/25 15:26:08 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>

/*start a new process in child and main results in 4 processes.*/
int main(int argc, char **argv)
{
	int id = fork();
	int id2 = fork();
	printf("Hello world from process with id1: %d id2: %d\n", id, id2);
	return (0);
}