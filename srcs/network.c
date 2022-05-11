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
	int				ret = 1;

	getifaddrs(&addrs);
	tmp = addrs;
	while (tmp)
	{
		if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET)
		{
			if (malcolm->options.v)
				printf("checking for interface %s...\n", tmp->ifa_name);
			uint32_t netip = ((struct sockaddr_in *)tmp->ifa_addr)->sin_addr.s_addr;
			uint32_t netmask = ((struct sockaddr_in *)tmp->ifa_netmask)->sin_addr.s_addr;
			if ((netip & netmask) == (malcolm->source.inet_ip & netmask) && (netip & netmask) == (malcolm->target.inet_ip & netmask))
			{
				int				i = 1;
				struct ifaddrs	*tmpp = addrs;
				while (tmpp)
				{
					if (tmpp->ifa_addr && !ft_memcmp(tmpp->ifa_name, tmp->ifa_name, ft_strlen(tmpp->ifa_name)) && tmpp->ifa_addr->sa_family == AF_PACKET)
					{
						printf("Found available interface: %s\n", tmp->ifa_name);
						ft_memcpy(malcolm->arp_ip, &((struct sockaddr_in *)tmp->ifa_addr)->sin_addr, IPV4_LENGTH);
						ft_memcpy(malcolm->arp_mac, ((struct sockaddr_ll*)(tmpp->ifa_addr))->sll_addr, MAC_LENGTH);
						if (malcolm->options.v)
						{
							printf("	ip: ");
							print_arp_ip(malcolm->arp_ip);
							printf("	mac: ");
							print_arp_mac(malcolm->arp_mac);
						}
						malcolm->ifindex = i;
						ret = 0;
						break ;
					}
					i++;
					tmpp = tmpp->ifa_next;
				}
				if (tmpp)
					break ;
				else if (malcolm->options.v)
					printf("	%s is not of type AF_PACKET\n", tmp->ifa_name);
			}
			else if (malcolm->options.v)
			{
				if ((netip & netmask) != (malcolm->source.inet_ip & netmask))
					printf("	%s not in interface ip range\n", malcolm->source.ip);
				if ((netip & netmask) != (malcolm->target.inet_ip & netmask))
					printf("	%s not in interface ip range\n", malcolm->target.ip);
			}
		}
		tmp = tmp->ifa_next;
	}
	freeifaddrs(addrs);
	if (ret)
		dprintf(STDERR_FILENO, "%s: No suitable available interface found.\n", PRG_NAME);
	return (ret);
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

