/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/14 22:44:01 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/16 03:10:32 by mamartin         ###   ########.fr       */
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
	int i;
	int j;

	for (i = 1; i < count; i++)
	{
		if (args[i][0] == '-') // it's an option
		{
			for (j = 1; args[i][j] != '\0'; j++)
			{
				switch (args[i][j])
				{
					case 'h':
						print_usage(0);
						break;
					case 'v':
						g_params.options.verbose = 1;
						break;
					default:
						print_usage(args[i][1]);
						break;
				}
			}
		}
		else // it's an address
		{
			if (address != NULL)
				exit_error("usage error: Only one destination address must be provided\n");
			address = args[i];
		}
	}

	if (!address)
		exit_error("usage error: Destination address required\n");
	return address;
}

void print_usage(char option)
{
	if (option)
		dprintf(STDERR_FILENO, "ping: invalid option -- \'%c\'\n", option);

	dprintf(STDERR_FILENO, "\nUsage\n  ping [options] <destination>\n");
	dprintf(STDERR_FILENO, "\nOptions:\n");
	dprintf(STDERR_FILENO, "  <destination>\tdns name or ip address\n");
	dprintf(STDERR_FILENO, "  -h\t\tprint help and exit\n");
	dprintf(STDERR_FILENO, "  -v\t\tverbose output\n");
	exit(2);
}
