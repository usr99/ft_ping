/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/14 22:44:01 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/26 20:23:26 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>

#include "ft_ping.h"
#include "args.h"

extern t_ping_params g_params;

char* parse_arguments(int argc, char** argv)
{
	static t_expected_opts valid_options[N_OPTIONS_SUPPORTED] = {
		{ .name = 'h', .has_param = false },
		{ .name = 'v', .has_param = false },
		{ .name = 'q', .has_param = false },
		{ .name = 'n', .has_param = false },
		{ .name = 'c', .has_param = true, .paramtype = PARAM_T_INT64 },
		{ .name = 'l', .has_param = true, .paramtype = PARAM_T_INT64 },
		{ .name = 't', .has_param = true, .paramtype = PARAM_T_INT64 },
		{ .name = 'w', .has_param = true, .paramtype = PARAM_T_INT64 }
	};

	char* address = NULL;
	t_argument arg;
	long long val;
	int ret;
	
	g_params.options.count = -1;
	g_params.options.ttl = -1;
	g_params.options.preload = 1;
	g_params.options.deadline = -1;

	while ((ret = ft_getarg(argc, argv, valid_options, N_OPTIONS_SUPPORTED, &arg)) == 0)
	{
		switch (arg.type)
		{
			case ARG_T_OPTION:
				switch (arg.name)
				{
					case 'h':
						print_usage(); break;
					case 'n':
						g_params.options.numeric_output = 1;
						break;
					case 'v':
						g_params.options.verbose = 1;
						break;
					case 'q':
						g_params.options.quiet = 1;
						break;
					case 't':
						val = *(long long*)arg.value;
						free(arg.value);
						if (val < 1 || val > UINT8_MAX)
							exit_error("invalid argument for -t option: out of range: 1 <= value <= 255");
						g_params.options.ttl = val;
						break;
					case 'c':
						val = *(long long*)arg.value;
						free(arg.value);
						if (val < 1 || val > INT64_MAX)
							exit_error("invalid argument for -c option: out of range: 1 <= value <= 9223372036854775807");
						g_params.options.count = val;
						break;
					case 'l':
						val = *(long long*)arg.value;
						free(arg.value);
						if (val < 1 || val > UINT16_MAX)
							exit_error("invalid argument for -l option: out of range: 1 <= value <= 65535");
						g_params.options.preload = val;
						break;
					case 'w':
						val = *(long long*)arg.value;
						free(arg.value);
						if (val < 0 || val > INT32_MAX)
							exit_error("invalid argument for -w option: out of range: 0 <= value <= 2147483647");
						g_params.options.deadline = val;
						break;
					default: // should never happen
						break;
				}
				break ;
			case ARG_T_PARAMETER:
				if (address)
					exit_error("usage error: Only one destination address must be provided");
				address = arg.value;
				break ;
			case ARG_T_ERROR:
				switch (arg.errtype)
				{
					case ERR_BAD_OPTION:
						dprintf(STDERR_FILENO, "%s: invalid option -- \'%c\'\n",
							g_params.program_name, arg.name);
						print_usage(); break;
					case ERR_MISSING_PARAM:
						dprintf(STDERR_FILENO, "%s: option requires an argument -- \'%c\'\n",
							g_params.program_name, arg.name);
						print_usage(); break;
					case ERR_BAD_PARAM_TYPE:
						dprintf(STDERR_FILENO, "%s: invalid argument: \'%s\'\n",
							g_params.program_name, (char*)arg.value);
						break;
					default:
						break; // should never happen
				}
				break ;
		}
	}

	if (ret == -2) // code to indicate memory allocation failure
		exit_error("Out of memory");
	
	if (!address)
		exit_error("usage error: Destination address required");
	return address;
}

void print_usage()
{
	dprintf(STDERR_FILENO, "\nUsage\n  %s [options] <destination>\n", g_params.program_name);
	dprintf(STDERR_FILENO, "\nOptions:\n");
	dprintf(STDERR_FILENO, "  <destination>\tdns name or ip address\n");
	dprintf(STDERR_FILENO, "  -c <count>\tstop after <count> replies\n");
	dprintf(STDERR_FILENO, "  -h\t\tprint help and exit\n");
	dprintf(STDERR_FILENO, "  -l <preload>\tsend <preload> number of packages while waiting replies\n");
	dprintf(STDERR_FILENO, "  -n\t\tnumeric output only, no reverse dns lookup\n");
	dprintf(STDERR_FILENO, "  -q\t\tquiet output\n");
	dprintf(STDERR_FILENO, "  -t <ttl>\tdefine time to live\n");
	dprintf(STDERR_FILENO, "  -v\t\tverbose output\n");
	dprintf(STDERR_FILENO, "  -w <deadline>\treply wait <deadline> in seconds\n");
	exit(2);
}
