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
	printf("Hello world !\n");
	return (0);
}
