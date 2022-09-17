/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/14 22:44:01 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/16 23:27:19 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>

#include "ft_ping.h"
#include "args.h"

extern t_ping_params g_params;

char* parse_arguments(char** args, int count)
{
	char* address = NULL;
	int done = 0;
	int i;
	int j;

	g_params.options.interval = 1;
	g_params.options.count = -1;
	g_params.options.ttl = -1;
	g_params.options.preload = 1;
	g_params.options.deadline = -1;

	for (i = 1; i < count; i++)
	{
		if (args[i][0] == '-') // it's an option
		{
			done = 0;
			for (j = 1; args[i][j] != '\0' && !done; j++)
			{
				switch (args[i][j])
				{
					case 'h':
						print_usage();
						break;
					case 'v':
						g_params.options.verbose = 1;
						break;
					case 'q':
						g_params.options.quiet = 1;
						break;
					case 't':
						g_params.options.ttl = parse_option_value(args, count, &i, j+1, 0, UINT8_MAX);	
						done = 1;
						break;
					case 'c':
						g_params.options.count = parse_option_value(args, count, &i, j+1, 1, INT64_MAX);
						done = 1;
						break;
					case 'w':
						g_params.options.deadline = parse_option_value(args, count, &i, j+1, 0, INT32_MAX);
						done = 1;
						break;
					default:
						dprintf(STDERR_FILENO, "ping: invalid option -- \'%c\'\n", args[i][j]);
						print_usage();
						break;
				}
			}
		}
		else // it's an address
		{
			if (address != NULL)
				exit_error("usage error: Only one destination address must be provided");
			address = args[i];
		}
	}
	
	if (!address)
		exit_error("usage error: Destination address required");
	return address;
}

long parse_option_value(char **args, int count, int* start_str, int start_char, long min, long max)
{
	long value;
	int i;
	int j;

	for (i = *start_str; i < count; i++)
	{
		for (j = start_char; args[i][j] != '\0'; j++)
		{
			if (ft_isdigit(args[i][j]) || args[i][j] == '-')
			{
				value = atol(args[i] + j);

				if (value < min || value > max)
				{
					dprintf(STDERR_FILENO, "ping: invalid argument: \'%s\': out of range: ", args[i] + start_char);
					dprintf(STDERR_FILENO, "%ld <= value <= %ld\n", min, max);
					exit(2);					
				}

				for (j = j + 1; ft_isdigit(args[i][j]); j++);
				if (args[i][j] != '\0')
				{
					dprintf(STDERR_FILENO, "ping: invalid argument: \'%s\'\n", args[i] + start_char);
					exit(2);
				}

				*start_str = i;
				return value;
			}
			else if (args[i][j] != ' ')
			{
				dprintf(STDERR_FILENO, "ping: invalid argument: \'%s\'\n", args[i] + j);
				exit(2);
			}
		}
		start_char = 0;
	}
	dprintf(STDERR_FILENO, "ping: option requires an argument -- \'%c\'\n", 't');
	print_usage();
	return 0;
}

void print_usage()
{
	dprintf(STDERR_FILENO, "\nUsage\n  ping [options] <destination>\n");
	dprintf(STDERR_FILENO, "\nOptions:\n");
	dprintf(STDERR_FILENO, "  <destination>\tdns name or ip address\n");
	dprintf(STDERR_FILENO, "  -h\t\tprint help and exit\n");
	dprintf(STDERR_FILENO, "  -v\t\tverbose output\n");
	exit(2);
}
