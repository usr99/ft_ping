/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/11 17:56:54 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/13 17:18:32 by mamartin         ###   ########.fr       */
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

typedef struct s_icmp
{
	struct icmphdr header;
	uint8_t payload[PAYLOAD_SIZE];
} t_icmp;

typedef struct s_reply
{
	struct iphdr ip_header;
	struct icmphdr icmp_header;
	struct timeval timestamp;
	in_addr_t source_addr;
} t_reply;

t_icmp create_icmp_packet(pid_t pid, int count);
uint16_t compute_checksum(uint16_t* data);
int receive_reply(int sock, t_reply* reply);
t_ping_request* get_request(t_reply* reply, struct sockaddr_in* addr, pid_t pid);
t_reply_code get_reply_state(t_ping_request* req, t_reply* reply);

#endif
