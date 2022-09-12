/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/10 17:36:35 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/12 02:48:34 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>

#include "ft_ping.h"
#include "libft.h"

void exit_error(const char* msg)
{
	ft_putstr_fd(msg, STDERR_FILENO);
	exit(EXIT_FAILURE);
}

float get_duration_ms(struct timeval* from)
{
	struct timeval now;
	gettimeofday(&now, NULL);

	float seconds = (now.tv_sec - from->tv_sec) * 1000;
	float milliseconds = (now.tv_usec - from->tv_usec) / 1000.f;
	return seconds + milliseconds;
}

void log_reply(t_reply* reply, const char* hostname, const char* address)
{
	printf("%ld bytes from %s (%s): icmp_seq=%d ttl=%d time=%.3fms\n",
		sizeof(t_icmp),
		hostname,
		address,
		reply->packet->header.un.echo.sequence,
		reply->ipheader->ttl,
		get_duration_ms(reply->timestamp)
	);
}
