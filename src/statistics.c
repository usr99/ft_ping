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

t_statistics compute_statistics(t_list* requests)
{
	t_ping_request* data;
	t_list* node;
	t_statistics st = { 0 };

	st.min = -1.f;
	st.max = -1.f;

	/*
	** Compute statistics
	** count of sent/received packets
	** min, avg and max round-trip time
	*/
	for (node = requests; node != NULL; node = node->next)
	{
		data = (t_ping_request*)node->content;
		
		if (data->state != WAITING_REPLY)
			st.replies[data->state]++;
		if (data->state != DUPLICATE)
			st.sent++;

		if (st.min == -1.f || data->elapsed_time < st.min)
			st.min = data->elapsed_time;
		if (st.max == -1.f || data->elapsed_time > st.max)
			st.max = data->elapsed_time;

		st.avg += data->elapsed_time;
	}
	if (st.sent + st.replies[DUPLICATE] != 0)
		st.avg /= (st.sent + st.replies[DUPLICATE]);

	st.loss = 100 * (st.sent - st.replies[SUCCESS]) / st.sent;
	return st;
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
	t_statistics stats;
	float mdev;

	stats = compute_statistics(requests);
	mdev = compute_standard_deviation(requests, stats.avg, stats.sent);

	if (g_params.hostname)
		printf("\n--- %s ping statistics ---\n", g_params.hostname);
	else
		printf("\n--- %s ping statistics ---\n", destination);

	printf("%d packets transmitted, %d received", stats.sent, stats.replies[SUCCESS]);
	if (stats.replies[DUPLICATE])
		printf(", +%d duplicates", stats.replies[DUPLICATE]);
	if (stats.replies[CORRUPTED])
		printf(", +%d corrupted", stats.replies[CORRUPTED]);
	if (stats.replies[ICMP_ERR])
		printf(", +%d errors", stats.replies[ICMP_ERR]);
	printf(", %d%% packet loss, time %dms\n", stats.loss, (int)get_duration_ms(start_time));

	if (stats.replies[SUCCESS] != 0)
	{
		printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
			stats.min, stats.avg, stats.max, mdev
		);
	}
	return stats.replies[SUCCESS];
}

void print_stats_sigquit(int signum)
{
	t_statistics stats;
	float ewma;

	(void)signum;

	stats = compute_statistics(g_params.requests);
	ewma = compute_exponential_moving_avg(g_params.requests);

	dprintf(STDERR_FILENO, "%d/%d packets, %d%% loss", stats.replies[SUCCESS], stats.sent, stats.loss);
	if (stats.replies[SUCCESS] != 0)
	{
		dprintf(STDERR_FILENO, ", min/avg/ewma/max = %.3f/%.3f/%.3f/%.3f ms",
			stats.min, stats.avg, ewma, stats.max);
	}
	dprintf(STDERR_FILENO, "\n");
}
