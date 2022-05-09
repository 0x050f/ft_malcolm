#include "ft_malcolm.h"

int			check_mac_addr(char *mac_addr)
{
	if (ft_strlen(mac_addr) != 17)
		return (1);
	for (int i = 0; mac_addr[i]; i++)
	{
		if (i % 3 == 2 && mac_addr[i] != ':')
			return (1);
		else if (i % 3 != 2 && (mac_addr[i] < '0' ||
(mac_addr[i] > '9' && (mac_addr[i] < 'a' || mac_addr[i] > 'f'))))
			return  (1);
	}
	return (0);
}

int			get_interface(t_malcolm *malcolm)
{
	struct ifaddrs	*addrs;
	struct ifaddrs	*tmp;
	int				i = 0;
	int				ret = 1;

	getifaddrs(&addrs);
	tmp = addrs;
	while (tmp)
	{
		if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET &&
!(tmp->ifa_flags & IFF_LOOPBACK)) { // avoid 127.0.0.1
			printf("Found available interface: %s\n", tmp->ifa_name);
			ft_memcpy(&malcolm->ifa, tmp, sizeof(struct ifaddrs));
			malcolm->ifindex = i;
			ret = 0;
			break ;
		}
		tmp = tmp->ifa_next;
		i++;
	}
	freeifaddrs(addrs);
	if (ret)
		dprintf(STDERR_FILENO, "%s: No suitable available interface found.\n", PRG_NAME);
	return (ret);
}

void		listen_arp_broadcast(t_malcolm *malcolm)
{
	t_arp_packet		*packet;
	unsigned char		buffer[ETH2_HEADER_LEN + sizeof(t_arp_packet)];

	if(recvfrom(malcolm->sockfd, &buffer, sizeof(buffer), 0, NULL, NULL) <= 0)
		return ;
	packet = (void *)&buffer + ETH2_HEADER_LEN;
	printf("\n");
	printf("An ARP request has been broadcast.\n");
	printf("	mac address of request: %02x:%02x:%02x:%02x:%02x:%02x\n", packet->sender_mac[0], packet->sender_mac[1], packet->sender_mac[2], packet->sender_mac[3], packet->sender_mac[4], packet->sender_mac[5]);
	printf("	IP address of request: %d.%d.%d.%d\n", packet->sender_ip[0], packet->sender_ip[1], packet->sender_ip[2], packet->sender_ip[3]);
}

int			init_malcolm(t_malcolm *malcolm, char *argv[])
{
	int				i;
	t_machine		*machines[2] = {&(malcolm->source), &(malcolm->target)};

	for (i = 1; i <= 4; i++)
	{
		if (i % 2)
		{
			(*machines)->ip = argv[i];
			if (((*machines)->inet_ip  = inet_addr(argv[i])) == INADDR_NONE)
				goto error;
		}
		else
		{
			(*machines)->mac = argv[i];
			if (check_mac_addr(argv[i]))
				goto error;
		}
	}
	malcolm->sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
	if (malcolm->sockfd < 0)
	{
		dprintf(STDERR_FILENO, "%s: socket: Operation not permitted\n", PRG_NAME);
		return (1);
	}
	if (get_interface(malcolm))
		return (1);
	malcolm->sockaddr.sll_family = AF_PACKET;
	malcolm->sockaddr.sll_protocol = htons(ETH_P_ARP);
	malcolm->sockaddr.sll_ifindex = malcolm->ifindex;
	malcolm->sockaddr.sll_hatype = htons(ARPHRD_ETHER);
	malcolm->sockaddr.sll_pkttype = (PACKET_BROADCAST);
	malcolm->sockaddr.sll_halen = MAC_LENGTH;
	malcolm->sockaddr.sll_addr[6] = 0x00;
	malcolm->sockaddr.sll_addr[7] = 0x00;
	return (0);
	error:
		dprintf(STDERR_FILENO, "%s: unknown host or invalid IP address: (%s).\n", PRG_NAME, argv[i]);
	return (1);
}

int			main(int argc, char *argv[])
{
	t_malcolm	malcolm;

	if (argc != 5)
	{
		printf("Usage: %s source_ip source_mac address_target_ip target_mac_address\n", PRG_NAME);
		return (1);
	}
	if (init_malcolm(&malcolm, argv))
		return (1);
	listen_arp_broadcast(&malcolm);
	return (0);
}
