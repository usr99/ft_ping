/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/10 17:46:56 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/12 02:40:49 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _FT_PING_H_
# define _FT_PING_H_

# include <sys/time.h>
# include "icmp.h"

void sigint_handler(int signum);

void exit_error(const char* msg);
float get_duration_ms(struct timeval* from);
void log_reply(t_reply* reply, const char* hostname, const char* address);

#endif
