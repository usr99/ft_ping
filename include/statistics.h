/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   statistics.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/13 15:16:16 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/16 23:22:00 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _STATISTICS_H_
# define _STATISTICS_H_

# include "libft.h"

typedef enum e_reply_code
{
	DUPLICATE,
	CORRUPTED,
	ICMP_ERR,
	SUCCESS,
	WAITING_REPLY
} t_reply_code;

typedef struct s_statistics
{
	// packets
	int sent;
	int replies[4];
	int loss;

	// round-trip time
	float min;
	float max;
	float avg;
} t_statistics;

typedef struct s_ping_request
{
	int icmp_sequence;
	t_reply_code state;
	float elapsed_time;
} t_ping_request;

t_ping_request* push_new_node(t_list** requests, int seq);
t_list* get_stat(t_list* requests, int icmpseq);
t_statistics compute_statistics(t_list* requests);
float compute_standard_deviation(t_list* requests, float average, int nreplies);
float compute_exponential_moving_avg(t_list* requests);
void print_statistics(t_list* requests, const char* destination, struct timeval start_time);
void print_stats_sigquit(int signum);

#endif
