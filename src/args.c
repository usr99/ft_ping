/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/14 22:44:01 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/14 23:09:17 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>

#include "ft_ping.h"
#include "args.h"

char* parse_arguments(char** args, int count)
{
	char* address = NULL;
	int i;

	for (i = 1; i < count; i++)
	{
		if (args[i][0] == '-') // it's an option
		{
			switch (args[i][1])
			{
				case 'h':
					print_usage(0);
				case 'v':
					break;
				default:
					print_usage(args[i][1]);
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
