/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maweiss <maweiss@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 10:03:45 by maweiss           #+#    #+#             */
/*   Updated: 2024/06/03 12:39:54 by maweiss          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

//libft
# include <stddef.h>
# include <stdlib.h>
# include <stdarg.h>
# include <unistd.h>
# include <limits.h>
# include <ctype.h>
//current project
# include <stdio.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <string.h>
# include <errno.h>

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
	normal_case,
	here_doc
}				t_case;

typedef enum e_bool
{
	false,
	true
}				t_bool;

/* 1 = string, 2 = 2d-array, 3 = 3d-array, 0 = empty */
typedef struct s_list
{
	void			*cont;
	int				type;
	struct s_list	*next;
}				t_list;

typedef struct s_pipex {
	int		argc;
	char	**argv;
	char	**envp;
	char	**path;
	char	**cmds;
	int		*cmd_ret;
	char	***cmd_args;
	int		pipe[2][2];
	int		mode;
	char	*delimiter;
	char	*infile;
	char	*outfile;
	int		nb_cmds;
}				t_pipex;

// libft
int		ft_atoi(const char *nptr);
long	ft_atol(const char *nptr);
int		ft_countwords(char const *str, char c);
void	*ft_bzero(void *s, size_t n);
void	*ft_calloc(size_t nmemb, size_t size);
int		ft_isalnum(int c);
int		ft_isalpha(int c);
int		ft_isascii(int c);
int		ft_isdigit(int c);
int		ft_isprint(int c);
char	*ft_itoa(int nb);
void	*ft_memchr(const void *s, int c, size_t n);
int		ft_memcmp(const void *s1, const void *s2, size_t n);
void	*ft_memcpy(void *dest, const void *src, size_t n);
void	*ft_memmove(void *dest, const void *src, size_t n);
void	*ft_memset(void *s, int c, size_t n);
void	ft_putchar_fd(char c, int fd);
void	ft_putendl_fd(char *s, int fd);
void	ft_putnbr_fd(int n, int fd);
void	ft_putstr_fd(char *s, int fd);
char	**ft_split(char const *s, char c);
char	*ft_strchr(const char *s, int c);
char	*ft_strdup(char *src);
void	ft_striteri(char *s, void (*f)(unsigned int, char*));
char	*ft_strjoin(char const *s1, char const *s2);
size_t	ft_strlcat(char *dst, const char *src, size_t size);
size_t	ft_strlcpy(char *dst, const char *src, size_t size);
size_t	ft_strlen(const char *str);
char	*ft_strmapi(char const *s, char (*f)(unsigned int, char));
int		ft_strncmp(const char *s1, const char *s2, size_t n);
char	*ft_strnstr(const char *big, const char *little, size_t len);
char	*ft_strrchr(const char *s, int c);
char	*ft_strtrim(char const *s1, char const *set);
char	*ft_substr(char const *s, unsigned int start, size_t len);
int		ft_tolower(int c);
int		ft_toupper(int c);
t_list	*ft_lstnew(void *cont);
void	ft_lstadd_front(t_list **lst, t_list *new);
int		ft_lstsize(t_list *lst);
t_list	*ft_lstlast(t_list *lst);
void	ft_lstadd_back(t_list **lst, t_list *new);
void	ft_lstdelone(t_list *lst, void (*del)(void *));
void	ft_lstclear(t_list **lst, void (*del)(void *));
void	ft_lstiter(t_list *lst, void (*f)(void *));
t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *));
int		ft_printf(const char *s, ...);
int		ft_printf_err(const char *s, ...);
int		ft_pnb_b_fd(long nbr, char *base, int fd, int negp);
int		ft_pnb_b_fd_s(int nbr, char *base, int fd, int negp);
int		ft_putstr_fd_ret(char *s, int fd);
int		ft_putchar_fd_ret(char c, int fd);
char	*ft_get_next_line(int fd);
void	ft_putstr_non_printable(char *str, size_t n);
int		ft_abs(int a);
int		ft_free_2d(char **tofree);
int		ft_free_3d(char ***tofree);

// pipex
void	ft_init_env(t_pipex *pipex, int *argc, char **argv, char **envp);
void	ft_validate_args(t_pipex *pipex);
void	ft_parse_cmds(t_pipex *pipex);
char	**ft_grab_envp(char **envp);
t_list	*ft_pipex_lstnew(void *cont, int type);
char	*ft_search_cmd(t_pipex *pipex, int nbcmd);
int		ft_first_child(t_pipex *pipex);
int		ft_parent_process(t_pipex *pipex);
void	ft_cleanup(t_pipex *pipex);
int		ft_child(t_pipex *pipex, int nb_cmd);
int		ft_errhandle(t_pipex *pipex, int nb);

#endif
