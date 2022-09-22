/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/14 22:43:34 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/22 16:16:59 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _ARGS_H_
# define _ARGS_H_

#define N_OPTIONS_SUPPORTED 7

typedef struct s_options
{
	uint8_t verbose;
	uint8_t quiet;
	int64_t count;
	uint16_t preload;
	int16_t ttl;
	int32_t deadline;
	uint8_t numeric_output;
} t_options;

char* parse_arguments(int argc, char** argv);
long parse_option_value(char **args, int count, int* start_str, int start_char, long min, long max);
void print_usage();

#endif
