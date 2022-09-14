/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/11 17:56:39 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/14 19:02:32 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/time.h>
#include <netdb.h>
#include "ft_ping.h"
#include "statistics.h"

extern t_ping_params g_params;

t_icmp create_icmp_packet(pid_t pid, int count)
{
	t_icmp packet;

	packet.header.type = ICMP_ECHO;
	packet.header.code = 0;
	packet.header.un.echo.id = pid;
	packet.header.un.echo.sequence = count;

	struct timeval tv;
	gettimeofday(&tv, NULL);
	ft_memset(packet.payload, 0, PAYLOAD_SIZE);
	ft_memcpy(packet.payload, &tv, sizeof(tv));
	
	packet.header.checksum = 0;
	packet.header.checksum = compute_checksum((uint16_t*)&packet, sizeof(t_icmp));
	return packet;
}

uint16_t compute_checksum(uint16_t* data, size_t bytes)
{
	uint32_t sum = data[0];
	size_t i;

	for (i = 1; i < bytes / 2; i++)
	{
		sum += data[i];
		if (sum >= 0x10000)
			sum -= 0xFFFF;
	}
	return ~sum;
}

int receive_reply(int sock, t_reply* reply)
{
	struct msghdr msg;
	struct iovec iov;
	struct sockaddr_in address_buffer;
	char buffer[PACKET_SIZE];

	ft_memset(&msg, 0, sizeof(msg));
	iov.iov_base = buffer;
	iov.iov_len = PACKET_SIZE;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_name = &address_buffer;
	msg.msg_namelen = sizeof(struct sockaddr_in);

	ssize_t bytes_read = recvmsg(sock, &msg, MSG_TRUNC);
	if (bytes_read == -1)
		return -1;

	// char host[1024] = { '\0' };
	// char svc[1024] = { '\0' };
	// int ret = getnameinfo(
	// 	(struct sockaddr*)&address_buffer, sizeof(struct sockaddr_in),
	// 	host, sizeof(host),
	// 	svc, sizeof(svc), 0
	// );
	// printf("%d %s %s\n", ret, host, svc);

	reply->ip_header = *(struct iphdr*)iov.iov_base;
	reply->icmp_header = *(struct icmphdr*)(char*)(iov.iov_base + IPHEADER_SIZE);
	reply->timestamp = *(struct timeval*)(iov.iov_base + IPHEADER_SIZE + ICMPHEADER_SIZE);
	reply->source_addr = address_buffer.sin_addr.s_addr;
	return 0;
}

t_ping_request* get_request(t_reply* reply, struct sockaddr_in* addr, pid_t pid)
{
	t_list* node;
	t_ping_request* req;
	t_ping_request* duplicate;
	t_reply_code new_state;
	
	/* Check that the packet is ours */
	if (reply->source_addr != addr->sin_addr.s_addr)
		return NULL; // not sent by the pinged host
	if (reply->icmp_header.type != ICMP_ECHOREPLY)
		return NULL; // not a ping reply
	if (reply->icmp_header.un.echo.id != pid)
		return NULL; // reply for another ping process

	/* Check the icmp sequence */
	node = get_stat(g_params.requests, reply->icmp_header.un.echo.sequence);
	if (!node)
		return NULL; // icmp sequence doesn't match any of the requests sent
	req = (t_ping_request*)node->content;

	new_state = get_reply_state(req, reply);
	if (new_state == DUPLICATE)
	{
		/* Duplicate the ping request for further rtt calculations */
		duplicate = push_new_node(&g_params.requests, req->icmp_sequence);
		if (!duplicate)
			exit_error("alloc failed\n");
		duplicate->elapsed_time = get_duration_ms(reply->timestamp);
		duplicate->state = DUPLICATE;
	}
	else
	{
		/* Update ping request status */
		req->state = new_state;
		req->elapsed_time = get_duration_ms(reply->timestamp);
	}
	return req;
}

t_reply_code get_reply_state(t_ping_request* req, t_reply* reply)
{
	/* Check that we had not already received a reply for this ping request */
	if (req->state != WAITING_REPLY)
		return DUPLICATE;
	
	/* Compare ICMP checksums */
	t_icmp icmp_cpy;
	icmp_cpy.header = reply->icmp_header;
	ft_memset(icmp_cpy.payload, 0, PAYLOAD_SIZE);
	ft_memcpy(icmp_cpy.payload, &reply->timestamp, sizeof(struct timeval));
	icmp_cpy.header.checksum = 0;
	icmp_cpy.header.checksum = compute_checksum((uint16_t*)&icmp_cpy, sizeof(t_icmp));
	if (reply->icmp_header.checksum != icmp_cpy.header.checksum)
		return CORRUPTED;

	/* Compare IP checksums */
	uint8_t ip_cpy[PACKET_SIZE];
	ft_memcpy(ip_cpy, &reply->ip_header, sizeof(struct iphdr));
	ft_memcpy(ip_cpy + IPHEADER_SIZE, &icmp_cpy, sizeof(t_icmp));
	((struct iphdr*)ip_cpy)->check = 0;
	((struct iphdr*)ip_cpy)->check = compute_checksum((uint16_t*)ip_cpy, PACKET_SIZE);
	if (reply->ip_header.check != ((struct iphdr*)ip_cpy)->check)
		return CORRUPTED;

	return SUCCESS;
}
