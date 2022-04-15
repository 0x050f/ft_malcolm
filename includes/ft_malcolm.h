#ifndef FT_MALCOLM_H
# define FT_MALCOLM_H

# include <arpa/inet.h>
# include <netinet/in.h>
# include <stdio.h>
# include <sys/socket.h>
# include <unistd.h>

# define PRG_NAME "ft_malcolm"

typedef struct		s_machine
{
	char			*ip;
	char			*mac;
	in_addr_t		inet_ip;
}					t_machine;

typedef struct		s_malcolm
{
	t_machine		source;
	t_machine		target;
}					t_malcolm;

/* utils.c */
size_t	ft_strlen(const char *s);

#endif
