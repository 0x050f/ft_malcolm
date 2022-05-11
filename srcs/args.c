#include "ft_malcolm.h"

int			handle_options(int argc, char *argv[], int *i, t_malcolm *malcolm)
{
	int		j;
	int		k;
	char	options[NB_OPTIONS] = {'h', 'v'};

	(void)argc;
	k = 0;
	j = 1;
	while (argv[*i][j])
	{
		k = 0;
		while (k < NB_OPTIONS)
		{
			if (argv[*i][j] == options[k])
				break ;
			k++;
		}
		if (k == NB_OPTIONS)
			break ;
		else
		{
			if (options[k] == 'h')
			{
				show_help();
				malcolm->options.h = 1;
				return (0);
			}
			else if (options[k] == 'v')
				malcolm->options.v = 1;
		}
		j++;
	}
	if (k == NB_OPTIONS)
	{
		char option[2] = {0, 0};
		option[0] = argv[*i][j];
		args_error(PRG_NAME, ERR_INV_OPT, option, 0, 0);
		return (2);
	}
	return (0);
}

int			check_args(int argc, char *argv[], t_malcolm *malcolm)
{
	int		ret;

	ft_memset(malcolm, 0, sizeof(malcolm));
	if (argc < 5)
	{
		show_help();
		malcolm->options.h = 1;
		return (0);
	}
	ft_memset(&malcolm->options, 0, sizeof(t_options));
	for (int i = 1; i < argc - 4; i++)
	{
		if (*argv[i] == '-')
		{
			ret = handle_options(argc, argv, &i, malcolm);
			if (ret)
				return (ret);
		}
		else
		{
			show_help();
			return (1);
		}
	}
	return (0);
}
