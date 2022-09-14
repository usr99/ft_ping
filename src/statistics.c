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

float compute_average_rtt(t_list* requests, int* sent, int* recv, float* min, float* max)
{
	t_ping_request* data;
	t_list* node;
	float avg = 0.f;

	*sent = 0;
	*recv = 0;
	*min = -1.f;
	*max = -1.f;

	/*
	** Compute statistics
	** count of sent/received packets
	** min, avg and max round-trip time
	*/
	for (node = requests; node != NULL; node = node->next, (*sent)++)
	{
		data = (t_ping_request*)node->content;
		if (data->state == SUCCESS)
			(*recv)++;

		if (*min == -1.f || data->elapsed_time < *min)
			*min = data->elapsed_time;
		if (*max == -1.f || data->elapsed_time > *max)
			*max = data->elapsed_time;

		avg += data->elapsed_time;
	}
	if (*sent != 0)
		avg /= *sent;

	return avg;
}

float compute_standard_deviation(t_list* requests, float average, int req_sent)
{
	t_ping_request* data;
	t_list* node;
	float mdev = 0.f;

	/* Compute the standard deviation from the average round-trip time */
	for (node = requests; node != NULL; node = node->next)
	{
		data = (t_ping_request*)node->content;
		mdev += ft_fpow(data->elapsed_time - average, 2);
	}
	if (req_sent != 0)
		mdev = ft_fsqrt(mdev / req_sent);

	return mdev;
}

void print_statistics(t_list* requests, struct timeval start_time)
{
	int req_sent, res_recvd, loss_percentage;
	float min, max, avg, mdev;

	avg = compute_average_rtt(requests, &req_sent, &res_recvd, &min, &max);
	mdev = compute_standard_deviation(requests, avg, req_sent);
	loss_percentage = 100 * (req_sent - res_recvd) / req_sent;

	printf("\n--- %s ping statistics ---\n", g_params.hostname);
	printf(
		"%d packets transmitted, %d received, %d%% packet loss, time %dms\n",
		req_sent, res_recvd, loss_percentage, (int)get_duration_ms(start_time)
	);
	printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
		min, avg, max, mdev
	);
}
