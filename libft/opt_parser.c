/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   opt_parser.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/18 04:08:52 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/18 11:11:38 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <stdlib.h>
#include "opt_parser.h"
#include "mandatory.h"

int ft_getarg(int argc, char** argv, t_expected_opts* options, int count, t_argument* arg)
{
	static int argidx = 1;
	static int charidx = 0;

	char* curr;
	t_expected_opts* opt;

	if (argidx >= argc)
		return -1;

	if (!argv[argidx][charidx])
		jump_to_next_arg(&argidx, &charidx);
	if (argidx >= argc)
		return -1;

	curr = argv[argidx];
	if (charidx == 0)
	{
		if (curr[0] == '-') // it's an option
			charidx = 1;
		else // it's a program parameter
		{
			arg->type = ARG_T_PARAMETER;
			arg->info.param.value = curr;
			jump_to_next_arg(&argidx, &charidx);
			return 0;
		}
	}

	if (!(opt = find_option(curr[charidx], options, count)))
		set_error(arg, ERR_BAD_OPTION, curr[charidx]);
	else
	{
		arg->type = ARG_T_OPTION;
		arg->info.opt.name = opt->name;

		if (opt->has_param)
		{
			if (ft_strlen(curr) != (unsigned int)(charidx + 1)) // take value from what's left of the current string
				arg->info.opt.value = parse_option_parameter(curr + charidx + 1, opt->paramtype);
			else
			{
				if (++argidx < argc) // take value from the next argument string
					arg->info.opt.value = parse_option_parameter(argv[argidx], opt->paramtype);
				else
				{
					set_error(arg, ERR_MISSING_PARAM, opt->name);
					return 0;
				}
			}
			
			if (!arg->info.opt.value)
			{
				if (errno == ENOMEM)
					return -2;
				set_error(arg, ERR_BAD_PARAM_TYPE, opt->name);
			}

			jump_to_next_arg(&argidx, &charidx);
		}
		else
		{
			arg->info.opt.value = NULL;
			charidx++;
		}
	}

	return 0;
}

t_expected_opts* find_option(char optname, t_expected_opts* options, int count)
{
	int i;

	for (i = 0; i < count; i++)
	{
		if (options[i].name == optname)
			return options + i;
	}
	return NULL;
}

void* parse_option_parameter(const char* str, t_paramtype type)
{
	void* ptr;
	int i;

	switch (type)
	{
		case PARAM_T_INT32:
			for (i = (str[0] == '-' ? 1 : 0); str[i]; i++)
			{
				if (!ft_isdigit(str[i]))
					return NULL;
			}
			ptr = malloc(sizeof(int32_t));
			if (ptr)
				*(int*)ptr = ft_atoi(str);
			break;
		case PARAM_T_STRING:
			return ft_strdup(str);
		default:
			return NULL; // should never happen
	}
	return ptr;
}

void jump_to_next_arg(int* arg, int* character)
{
	(*arg)++;
	*character = 0;
}

void set_error(t_argument* arg, t_errtype type, char name)
{
	arg->type = ARG_T_ERROR;
	arg->info.err.type = type;
	arg->info.err.name = name;
}
