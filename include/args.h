/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/14 22:43:34 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/16 19:29:34 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _ARGS_H_
# define _ARGS_H_

char* parse_arguments(char** args, int count);
int parse_option_value(char **args, int count, int* start_str, int* start_char);
void print_usage();

#endif
