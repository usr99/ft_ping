/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/11 17:56:54 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/16 02:43:03 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _ICMP_H_
# define _ICMP_H_

# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include "statistics.h"

# define IPHEADER_SIZE 20
# define ICMPHEADER_SIZE 8
# define PAYLOAD_SIZE 56
# define PACKET_SIZE 84

typedef enum e_msg_status
{
	NOT_OURS,
	OK,
	BAD_CHECKSUM
} t_msg_status;

typedef struct s_icmp_msg
{
	struct iphdr* ip;
	struct icmphdr* icmp;
	void* payload;
	t_msg_status status;
} t_icmp_msg;

typedef struct s_icmp_echo
{
	struct icmphdr header;
	uint8_t payload[PAYLOAD_SIZE];
} t_icmp_echo;

t_icmp_echo create_echo_message(pid_t pid, int count);
uint16_t compute_checksum(uint16_t* data, size_t bytes);
t_msg_status compare_checksums(struct icmphdr* icmp);
int recv_icmp_message(int sock, t_icmp_msg* message);
t_msg_status validate_message(char* buffer);
t_msg_status validate_error_message(char* buffer);

#endif
