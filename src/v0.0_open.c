/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 18:48:15 by maweiss           #+#    #+#             */
/*   Updated: 2024/05/22 18:58:31 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft/libft.h"
#include <fcntl.h>
#include <stdlib.h>



int	main(void)
{
	int		fd;
	char	*line;
	char	*tmp;

	fd = open("../infile", O_RDONLY);
	line = ft_get_next_line(fd);
	while(line)
	{
		ft_printf("%s", line);
		tmp = line;
		line = ft_get_next_line(fd);
		free(tmp);
	}
	close(fd);
	return (0);
}
