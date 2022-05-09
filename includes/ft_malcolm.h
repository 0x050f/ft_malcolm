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
# include <sys/socket.h>
# include <unistd.h>

# define PRG_NAME "ft_malcolm"
# define MAC_LENGTH 6
# define IPV4_LENGTH 4

# define ETH2_HEADER_LEN 14

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
}						t_machine;

typedef struct			s_malcolm
{
	t_machine			source;
	t_machine			target;
	struct sockaddr_ll	sockaddr;
	int					sockfd;
	struct ifaddrs		ifa;
	int					ifindex;
}						t_malcolm;

/* utils.c */
size_t	ft_strlen(const char *s);
void	*ft_memcpy(void *dst, const void *src, size_t n);

#endif
