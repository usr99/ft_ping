/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/11 17:56:39 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/16 03:18:02 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/time.h>
#include <netdb.h>
#include <errno.h>

#include "ft_ping.h"
#include "statistics.h"

extern t_ping_params g_params;

t_icmp_echo create_echo_message(pid_t pid, int count)
{
	t_icmp_echo msg;

	msg.header.type = ICMP_ECHO;
	msg.header.code = 0;
	msg.header.un.echo.id = pid;
	msg.header.un.echo.sequence = count;

	struct timeval tv;
	gettimeofday(&tv, NULL);
	ft_memset(msg.payload, 0, PAYLOAD_SIZE);
	ft_memcpy(msg.payload, &tv, sizeof(tv));
	
	msg.header.checksum = 0;
	msg.header.checksum = compute_checksum((uint16_t*)&msg, sizeof(t_icmp_echo));
	return msg;
}

uint16_t compute_checksum(uint16_t* data, size_t bytes)
{
	uint32_t sum = *data;
	size_t i;

	for (i = 1; i < bytes / 2; i++)
	{
		sum += data[i];
		if (sum >= 0x10000)
			sum -= 0xFFFF;
	}
	return ~sum;
}

t_msg_status compare_checksums(struct icmphdr* icmp)
{
	uint16_t oldcs;
	uint16_t newcs;

	oldcs = icmp->checksum;
	icmp->checksum = 0;
	newcs = compute_checksum((uint16_t*)icmp, sizeof(t_icmp_echo));
	icmp->checksum = oldcs;
	return (oldcs == newcs) ? OK : BAD_CHECKSUM;
}

int recv_icmp_message(int sock, t_icmp_msg* message)
{
	struct msghdr msg = { 0 };
	struct iovec iov = { 0 };
	struct sockaddr_in addrbuf = { 0 };
	char payload[1000] = { 0 };

	/* Fill message buffer before recv call */
	ft_memset(&msg, 0, sizeof(msg));
	iov.iov_base = payload;
	iov.iov_len = 1000;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_name = &addrbuf;
	msg.msg_namelen = sizeof(struct sockaddr_in);

	ssize_t bytes_recvd = recvmsg(sock, &msg, MSG_TRUNC);
	if (bytes_recvd == -1)
	{
		if (errno == EAGAIN)
			return -1;
		exit_error("read error");
	}
	else if (bytes_recvd < PACKET_SIZE)
		return -1; // packet is too short

	message->status = validate_message(payload);
	if (message->status == NOT_OURS)
		return -1;
	
	message->ip = (struct iphdr*)malloc(PACKET_SIZE);
	if (!message->ip)
		exit_error("alloc failed");
	ft_memcpy(message->ip, payload, PACKET_SIZE);
	message->icmp = (struct icmphdr*)((char*)message->ip + IPHEADER_SIZE);
	message->payload = (void*)((char*)message->icmp + ICMPHEADER_SIZE);
	return message->icmp->type;
}

t_msg_status validate_message(char* buffer)
{
	t_icmp_msg msg;
	t_msg_status st;

	msg.ip = (struct iphdr*)buffer;
	if (msg.ip->protocol != IPPROTO_ICMP)
		return NOT_OURS; // not ICMP protocol

	msg.icmp = (struct icmphdr*)(buffer + IPHEADER_SIZE);
	if (msg.icmp->type == ICMP_ECHO)
		return NOT_OURS; // ignore icmp echo requests

	if (msg.icmp->type == ICMP_ECHOREPLY)
	{
		if (msg.ip->saddr != g_params.address->sin_addr.s_addr)
			return NOT_OURS; // not sent by the pinged host
	}
	else
	{
		/* ICMP error messages contain the original message as payload */
		st = validate_error_message(buffer);
		if (st != OK)
			return st; // original message is not valid
		msg.icmp = (struct icmphdr*)(buffer + IPHEADER_SIZE * 2 + ICMPHEADER_SIZE);
	}

	if (msg.icmp->un.echo.id != g_params.pid)
		return NOT_OURS; // message for another ping process

	return compare_checksums((struct icmphdr*)(buffer + IPHEADER_SIZE));
}

t_msg_status validate_error_message(char* buffer)
{
	t_icmp_msg msg;

	msg.ip = (struct iphdr*)(buffer + IPHEADER_SIZE + ICMPHEADER_SIZE);
	if (msg.ip->protocol != IPPROTO_ICMP)
		return NOT_OURS; // not ICMP protocol
	if (msg.ip->daddr != g_params.address->sin_addr.s_addr)
		return NOT_OURS; // original destination is not the host we're pinging

	msg.icmp = (struct icmphdr*)(buffer + IPHEADER_SIZE * 2 + ICMPHEADER_SIZE);
	if (msg.icmp->type != ICMP_ECHO)
		return NOT_OURS; // we sent only echo requests so it's not our error

	return OK;
}
