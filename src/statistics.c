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

t_ping_request* push_new_node(t_list** ping_request, int seq)
{
	t_ping_request* data;

	data = ft_calloc(1, sizeof(t_ping_request));
	if (!data)
		return NULL;
	data->state = WAITING_REPLY;
	data->elapsed_time = 0;
	data->icmp_sequence = seq;

	t_list* node = ft_lstnew(data);
	if (!node)
		return NULL;
	
	ft_lstadd_front(ping_request, node);
	return data;
}

t_list* get_stat(t_list* requests, int icmpseq)
{
	t_list* req;
	t_ping_request* data;

	for (req = requests; req != NULL; req = req->next)
	{
		data = (t_ping_request*)req->content;
		if (data->icmp_sequence == icmpseq)
			break ;
	}
	return req;
}

float compute_average_rtt(t_list* requests, int* sent, int* recv, float* min, float* max)
{
	t_ping_request* data;
	t_list* node;
	float avg = 0.f;
	int n_duplicates = 0;

	*sent = 0;
	*recv = 0;
	*min = -1.f;
	*max = -1.f;

	/*
	** Compute statistics
	** count of sent/received packets
	** min, avg and max round-trip time
	*/
	for (node = requests; node != NULL; node = node->next)
	{
		data = (t_ping_request*)node->content;
		if (data->state == SUCCESS)
			(*recv)++;
		if (data->state != DUPLICATE)
			(*sent)++;
		else
			n_duplicates++;

		if (*min == -1.f || data->elapsed_time < *min)
			*min = data->elapsed_time;
		if (*max == -1.f || data->elapsed_time > *max)
			*max = data->elapsed_time;

		avg += data->elapsed_time;
	}
	if (*sent + n_duplicates != 0)
		avg /= (*sent + n_duplicates);

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

float compute_exponential_moving_avg(t_list* requests)
{
	t_ping_request* data;
	t_list* node;
	float ewma = 0.f;

	/* Compute the Exponentially Weighted Moving Average (EWMA) of RTTs */
	for (node = requests; node != NULL; node = node->next)
	{
		data = (t_ping_request*)node->content;
		ewma = 0.9 * ewma + 0.1 * data->elapsed_time;
	}
	return ewma;
}

int print_statistics(t_list* requests, const char* destination, struct timeval start_time)
{
	int req_sent, res_recvd, loss_percentage;
	float min, max, avg, mdev;

	avg = compute_average_rtt(requests, &req_sent, &res_recvd, &min, &max);
	mdev = compute_standard_deviation(requests, avg, req_sent);
	loss_percentage = 100 * (req_sent - res_recvd) / req_sent;

	if (g_params.hostname)
		printf("\n--- %s ping statistics ---\n", g_params.hostname);
	else
		printf("\n--- %s ping statistics ---\n", destination);
	printf(
		"%d packets transmitted, %d received, %d%% packet loss, time %dms\n",
		req_sent, res_recvd, loss_percentage, (int)get_duration_ms(start_time)
	);

	if (res_recvd != 0)
	{
		printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
			min, avg, max, mdev
		);
	}
	return res_recvd;
}

void print_stats_sigquit(int signum)
{
	int req_sent, res_recvd, loss_percentage;
	float min, max, avg, ewma;

	(void)signum;

	avg = compute_average_rtt(g_params.requests, &req_sent, &res_recvd, &min, &max);
	ewma = compute_exponential_moving_avg(g_params.requests);
	loss_percentage = 100 * (req_sent - res_recvd) / req_sent;

	printf("%d/%d packets, %d%% loss", res_recvd, req_sent, loss_percentage);
	if (res_recvd != 0)
		printf(", min/avg/ewma/max = %.3f/%.3f/%.3f/%.3f ms", min, avg, ewma, max);
	printf("\n");
}
