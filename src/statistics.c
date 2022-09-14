/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_request.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/13 15:16:06 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/13 16:59:53 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

#include "ft_ping.h"
#include "statistics.h"

extern t_ping_params g_params;

t_ping_request* push_new_node(t_list** ping_request)
{
	t_ping_request* data;

	data = ft_calloc(1, sizeof(t_ping_request));
	if (!data)
		return NULL;
	data->state = WAITING_REPLY;
	data->elapsed_time = 0;

	t_list* node = ft_lstnew(data);
	if (!node)
		return NULL;
	
	ft_lstadd_back(ping_request, node);
	return data;
}

t_list* get_stat(t_list* ping_request, int icmpseq)
{
	int count;

	for (count = 1; count != icmpseq && ping_request != NULL; count++)
		ping_request = ping_request->next;
	return ping_request;
}

void print_statistics(t_list* requests, struct timeval start_time)
{
	t_ping_request* data;
	t_list* node = requests;

	int loss_percentage;
	int req_sent;
	int res_recvd;

	float min = -1.f;
	float max = -1.f;
	float avg = 0.f;
	float mdev = 0.f;

	while (node)
	{
		data = (t_ping_request*)node->content;
		if (data->state == SUCCESS)
			res_recvd++;

		if (min == -1.f || data->elapsed_time < min)
			min = data->elapsed_time;
		if (max == -1.f || data->elapsed_time > max)
			max = data->elapsed_time;

		avg += data->elapsed_time;
		req_sent++;
		node = node->next;
	}

	loss_percentage = 100 * (req_sent - res_recvd) / req_sent;
	avg /= req_sent;

	printf("\n--- %s ping statistics ---\n", g_params.hostname);
	printf(
		"%d packets transmitted, %d received, %d%% packet loss, time %dms\n",
		req_sent, res_recvd, loss_percentage, (int)get_duration_ms(start_time)
	);
	printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
		min, avg, max, mdev
	);
}
