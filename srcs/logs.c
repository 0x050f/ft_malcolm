#include "ft_malcolm.h"

void		show_help(void)
{
	char *options[NB_OPTIONS][2] =
	{
		{"-h", "print help and exit"},
		{"-v", "verbose mode"},
	};

	printf("Usage: %s [options] <source_ip> <source_mac> <address_target_ip> <target_mac_address>\nOptions:\n", PRG_NAME);
	for (size_t i = 0; i < NB_OPTIONS; i++)
		printf("  %-18s %s\n", options[i][0], options[i][1]);
}

int			args_error(char *prg_name, int error, char *str, int range1, int range2)
{
	dprintf(STDERR_FILENO, "%s: ", prg_name);
	if (error == ERR_INV_OPT)
		dprintf(STDERR_FILENO, "invalid option -- '%s'\n", str);
	else if (error == ERR_INV_ARG || error == ERR_OOR_ARG)
	{
		dprintf(STDERR_FILENO, "invalid argument: '%s'", str);
		if (error == ERR_OOR_ARG)
			dprintf(STDERR_FILENO, ": out of range: %d <= value <= %d", range1, range2);
		dprintf(STDERR_FILENO, "\n");
	}
	else if (error == ERR_REQ_ARG)
		dprintf(STDERR_FILENO, "option requires an argument -- '%s'\n", str);
	return (2);
}

void		print_ethernet_header(struct ethhdr *ethhdr)
{
	printf("Ethernet header:\n");
	printf("	destination: ");
	print_arp_mac(ethhdr->h_dest);
	printf("	source: ");
	print_arp_mac(ethhdr->h_source);
	printf("	type: 0x%04x\n", ntohs(ethhdr->h_proto));
}

void		print_arp_header(struct arphdr *arp_hdr)
{
	printf("Address Resolution Protocol:\n");
	printf("	hardware type: %d\n", ntohs(arp_hdr->ar_hrd));
	printf("	protocol type: 0x%04x\n", ntohs(arp_hdr->ar_pro));
	printf("	hardware size: %d\n", arp_hdr->ar_hln);
	printf("	protocol size: %d\n", arp_hdr->ar_pln);
	printf("	op code: %d\n", ntohs(arp_hdr->ar_op));
}
