/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   statistics.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/13 15:16:16 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/14 17:10:35 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _STATISTICS_H_
# define _STATISTICS_H_

# include "libft.h"

typedef enum e_reply_code
{
	WAITING_REPLY,
	NOT_OURS,
	DUPLICATE,
	CORRUPTED,
	SUCCESS
} t_reply_code;

typedef struct s_ping_request
{
	t_reply_code state;
	float elapsed_time;
	int icmp_sequence;
} t_ping_request;

t_ping_request* push_new_node(t_list** requests, int seq);
t_list* get_stat(t_list* requests, int icmpseq);
float compute_average_rtt(t_list* requests, int* sent, int* recv, float* min, float* max);
float compute_standard_deviation(t_list* requests, float average, int req_sent);
int print_statistics(t_list* requests, struct timeval start_time);

#endif
