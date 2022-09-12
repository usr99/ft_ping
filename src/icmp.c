/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/11 17:56:39 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/12 02:44:52 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/time.h>
#include "icmp.h"
#include "libft.h"

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
	packet.header.checksum = compute_checksum((uint16_t*)&packet);
	return packet;
}

uint16_t compute_checksum(uint16_t* data)
{
	uint32_t sum = data[0];
	size_t i;

	for (i = 1; i < sizeof(t_icmp) / 2; i++)
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

	ft_memset(&msg, 0, sizeof(msg));
	iov.iov_len = PACKET_SIZE;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_name = &address_buffer;
	msg.msg_namelen = sizeof(address_buffer);

	iov.iov_base = malloc(PACKET_SIZE);
	if (iov.iov_base == NULL)
		return -1;

	ssize_t bytes_read = recvmsg(sock, &msg, MSG_TRUNC);
	if (bytes_read == -1)
		return -1;

	reply->source_addr = address_buffer.sin_addr.s_addr;
	reply->ipheader = (struct iphdr*)(char*)(iov.iov_base);
	reply->packet = (t_icmp*)(char*)(iov.iov_base + 20);
	reply->timestamp = (struct timeval*)reply->packet->payload;
	return 0;
}

int check_reply_type(t_reply* reply, struct sockaddr_in* addr, pid_t pid)
{
	if (reply->source_addr != addr->sin_addr.s_addr)
		return 0; // not sent by the pinged host
	if (reply->packet->header.type != ICMP_ECHOREPLY)
		return 0; // not a ping reply
	if (reply->packet->header.un.echo.id != pid)
		return 0; // reply for another ping process
	return 1;
}
