/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/10 17:36:35 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/16 03:18:12 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <netdb.h>

#include "ft_ping.h"

extern t_ping_params g_params;

void clean_all()
{
	if (g_params.sockfd)
		close(g_params.sockfd);
	if (g_params.address)
		free(g_params.address);
	if (g_params.hostname)
		free(g_params.hostname);
	if (g_params.requests)
		ft_lstclear(&g_params.requests, &free);
}

void exit_error(const char* message)
{
	clean_all();
	ft_putstr_fd("ping: ", STDERR_FILENO);
	ft_putendl_fd(message, STDERR_FILENO);
	exit(2);
}

void sigint_handler(int signum)
{
	(void)signum;
	g_params.finished = 1;
}

float get_duration_ms(struct timeval from)
{
	struct timeval now;
	float seconds;
	float milliseconds;
	
	gettimeofday(&now, NULL);
	seconds = (now.tv_sec - from.tv_sec) * 1000;
	milliseconds = (now.tv_usec - from.tv_usec) / 1000.f;
	return seconds + milliseconds;
}
