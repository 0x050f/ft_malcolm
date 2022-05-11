#ifndef FT_MALCOLM_H
# define FT_MALCOLM_H

# include <arpa/inet.h>
# include <ifaddrs.h>
# include <linux/if_ether.h>
# include <linux/if_packet.h>
# include <netinet/in.h>
# include <net/if_arp.h>
# include <net/if.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <unistd.h>

# define PRG_NAME "ft_malcolm"
# define MAC_LENGTH 6
# define IPV4_LENGTH 4

# define PADDING 18

# define NB_OPTIONS 2

# define ERR_NB_DEST		1
# define ERR_INV_OPT		2
# define ERR_INV_ARG		3
# define ERR_OOR_ARG		4 /* OUT OF RANGE */
# define ERR_REQ_ARG		5

# define print_arp_mac(x) printf("%02x:%02x:%02x:%02x:%02x:%02x\n", x[0], x[1], x[2], x[3], x[4], x[5])
# define print_arp_ip(x) printf("%d.%d.%d.%d\n", x[0], x[1], x[2], x[3])

typedef struct			s_arp_packet
{
	struct arphdr		arp_hdr;
	unsigned char		sender_mac[MAC_LENGTH];
	unsigned char		sender_ip[IPV4_LENGTH];
	unsigned char		target_mac[MAC_LENGTH];
	unsigned char		target_ip[IPV4_LENGTH];
}						t_arp_packet;

typedef struct			s_machine
{
	char				*ip;
	char				*mac;
	in_addr_t			inet_ip;
	unsigned char		arp_mac[MAC_LENGTH];
	unsigned char		arp_ip[IPV4_LENGTH];
}						t_machine;

typedef struct			s_options
{
	int					h;
	int					v;
}						t_options;

typedef struct			s_malcolm
{
	t_machine			source;
	t_machine			target;
	struct sockaddr_ll	sockaddr;
	int					sockfd;
	struct ifaddrs		ifa;
	int					ifindex;
	t_options			options;
}						t_malcolm;

/* args.c */
int			check_args(int argc, char *argv[], t_malcolm *malcolm);

/* utils.c */
size_t		ft_strlen(const char *s);
void		*ft_memset(void *b, int c, size_t len);
int			ft_memcmp(const void *s1, const void *s2, size_t n);
void		*ft_memcpy(void *dst, const void *src, size_t n);
int			hex2int(const char *hex);
int			ft_atoi(const char *str);

#endif
