/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/10 16:48:55 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/12 02:44:31 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "ft_ping.h"
#include "libft.h"

uint8_t g_continue = 1;

int main(int argc, char **argv)
{
	int sock;
	struct addrinfo hint;
	struct addrinfo* results;

	if (argc != 2)
		exit_error("no ip provided\n");

	if (signal(SIGINT, sigint_handler) == SIG_ERR)
		exit_error("failed to set SIGINT handling behavior\n");

	sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sock == -1)
		exit_error("failed to create socket\n");

	ft_memset(&hint, 0, sizeof(struct addrinfo));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_RAW;
	hint.ai_protocol = IPPROTO_ICMP;
	hint.ai_flags = AI_CANONNAME;
	if (getaddrinfo(argv[1], NULL, &hint, &results) != 0)
	{
		close(sock);
		exit_error("failed to resolve host\n");
	}

	struct sockaddr_in* addr = (struct sockaddr_in*)results->ai_addr;
	char buffer[INET_ADDRSTRLEN] = { '\0' };

	if (inet_ntop(AF_INET, &addr->sin_addr, buffer, INET_ADDRSTRLEN) == NULL)
	{
		close(sock);
		freeaddrinfo(results);
		exit_error("failed to translate address in ASCII form\n");
	}

	const pid_t pid = getpid();
	uint16_t count = 1;
	printf("PING %s (%s) %d(%d) bytes of data.\n", results->ai_canonname, buffer, PAYLOAD_SIZE, PACKET_SIZE);
	while (g_continue)
	{
		/* Build ICMP packet for ECHO REQUEST */
		t_icmp packet = create_icmp_packet(pid, count);

		if (sendto(sock, &packet, sizeof(t_icmp), 0, (struct sockaddr*)addr, sizeof(struct sockaddr_in)) == -1)
			exit_error("failed to send message\n");

		t_reply reply;
		if (receive_reply(sock, &reply) == -1)
			exit_error("failed to receive message\n");

		if (check_reply_type(&reply, addr, pid))
			log_reply(&reply, results->ai_canonname, buffer);
		free((char*)reply.packet - IPHEADER_SIZE);

		sleep(1);
		count++;
	}

	close(sock);
	freeaddrinfo(results);
	return 0;
}

void sigint_handler(int signum)
{
	(void)signum;
	g_continue = 0;
}
