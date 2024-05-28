/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   01_first_fork.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 15:12:29 by maweiss           #+#    #+#             */
/*   Updated: 2024/05/25 15:27:01 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>

/*Split program into two processes resulting in two hello world statemenets.*/
int main(int argc, char **argv)
{
	int id = fork();
	printf("Hello world from id: %d\n", id);
	return (0);
}