/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/10 17:46:56 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/13 15:11:01 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _FT_PING_H_
# define _FT_PING_H_

# include <sys/time.h>
# include "icmp.h"

typedef struct s_ping_params
{
	int sockfd;
	pid_t pid;
	int icmp_count;
	struct sockaddr_in* address;
	char* hostname;
	int finished;
} t_ping_params;

void init_ping(t_ping_params* params, const char* destination);
void send_ping(int signum);
void log_reply(t_reply* reply, const char* hostname, const char* address);

void clean_all();
void exit_error(const char* message);
void sigint_handler(int signum);
float get_duration_ms(struct timeval from);

#endif
