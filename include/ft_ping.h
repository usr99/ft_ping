/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/10 17:46:56 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/15 01:30:01 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _FT_PING_H_
# define _FT_PING_H_

# include <sys/time.h>
# include "icmp.h"
# include "libft.h"

typedef struct s_options
{
	int verbose;
} t_options;

typedef struct s_ping_params
{
	t_options options;
	int sockfd;
	pid_t pid;
	int icmp_count;
	struct sockaddr_in* address;
	char* hostname;
	t_list* requests;
	int finished;
} t_ping_params;

/* ft_ping.c */
void init_ping(t_ping_params* params, const char* destination, char* addrname);
void send_ping(int signum);
void log_reply(t_reply* reply, t_ping_request* req, const char* address);

/* utils.c */
void clean_all();
void exit_error(const char* message);
void sigint_handler(int signum);
float get_duration_ms(struct timeval from);

#endif
