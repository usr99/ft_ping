/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/10 16:48:55 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/14 22:44:11 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#include "ft_ping.h"
#include "statistics.h"
#include "args.h"

t_ping_params g_params;

int main(int argc, char **argv)
{
	struct timeval start;
	char* destination_addr;
	char addrname[INET_ADDRSTRLEN];
	int replies_received;

	destination_addr = parse_arguments(argv, argc);
	if (signal(SIGALRM, send_ping) == SIG_ERR)
		exit_error("failed to set SIGALRM handling behavior\n");
	if (signal(SIGINT, sigint_handler) == SIG_ERR)
		exit_error("failed to set SIGINT handling behavior\n");
	if (signal(SIGQUIT, print_stats_sigquit) == SIG_ERR)
		exit_error("failed to set SIGQUIT handling behavior\n");

	ft_memset(&g_params, 0, sizeof(t_ping_params));
	init_ping(&g_params, destination_addr, addrname);

	gettimeofday(&start, NULL);
	send_ping(SIGALRM);
	while (!g_params.finished)
	{
		t_reply reply;
		if (receive_reply(g_params.sockfd, &reply) == 0)
		{
			t_ping_request* req = get_request(&reply, g_params.address, g_params.pid);
			if (req != NULL)
				log_reply(&reply, req, addrname);
		}
		else
		{
			if (errno != EAGAIN)
				exit_error("read error\n");
		}
	}

	replies_received = print_statistics(g_params.requests, destination_addr, start);
	clean_all();
	return replies_received ? 0 : 1;
}

void init_ping(t_ping_params* params, const char* destination, char* addrname)
{
	struct addrinfo* results;
	struct addrinfo hint;

	/* Create a raw socket for ICMP protocol */
	params->sockfd = socket(AF_INET, SOCK_RAW | SOCK_NONBLOCK, IPPROTO_ICMP);
	if (g_params.sockfd == -1)
		exit_error("failed to create socket\n");

	/* DNS lookup to find the address under the domain name */
	ft_memset(&hint, 0, sizeof(struct addrinfo));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_RAW;
	hint.ai_protocol = IPPROTO_ICMP;
	hint.ai_flags = AI_CANONNAME;
	if (getaddrinfo(destination, NULL, &hint, &results) != 0)
		exit_error("failed to resolve host\n");

	/* Copy address and hostname before freeing the results */
	params->address = malloc(sizeof(struct sockaddr_in));
	if (!params->address)
		exit_error("alloc failed\n");
	if (results->ai_addrlen != sizeof(struct sockaddr_in))
		exit_error("bad address structure\n");
	ft_memcpy(params->address, results->ai_addr, results->ai_addrlen);
	
	/* Handles the special case "ft_ping 0" which must ping localhost */
	if (params->address->sin_addr.s_addr == 0)
	{
		params->address->sin_addr.s_addr = 0x0100007F; // localhost (127.0.0.1)
		ft_strlcpy(addrname, "127.0.0.1", INET_ADDRSTRLEN);
	}
	else
	{
		/* Convert ip address into a string format */
		if (!inet_ntop(AF_INET, &g_params.address->sin_addr, addrname, INET_ADDRSTRLEN))
			exit_error("failed to convert address into a string format\n");
		/* Only set hostname if it's not equal to the ip address */
		if (ft_strncmp(results->ai_canonname, addrname, INET_ADDRSTRLEN) != 0)
		{
			params->hostname = ft_strdup(results->ai_canonname);
			if (!params->hostname)
				exit_error("alloc failed\n");
		}
	}
	freeaddrinfo(results);

	/* Initialize the last fields */
	params->pid = getpid();
	params->icmp_count = 1;
	params->requests = NULL;

	if (params->hostname)
		printf("PING %s", params->hostname);
	else
		printf("PING %s", destination);
	printf(" (%s) %d(%d) bytes of data.\n", addrname, PAYLOAD_SIZE, PACKET_SIZE);
}

void send_ping(int signum)
{
	(void)signum;
	if (g_params.finished)
		return ;

	t_icmp packet = create_icmp_packet(g_params.pid, g_params.icmp_count);
	if (sendto(g_params.sockfd, &packet, sizeof(t_icmp), 0, (struct sockaddr*)g_params.address, sizeof(struct sockaddr_in)) == -1)
		exit_error("failed to send message\n");

	if (!push_new_node(&g_params.requests, g_params.icmp_count))
		exit_error("alloc failed\n");

	g_params.icmp_count++;
	alarm(g_params.finished ? 0 : 1);
}

void log_reply(t_reply* reply, t_ping_request* req, const char* address)
{
	int precision = 1;

	if (req->state == SUCCESS)
	{
		if (req->elapsed_time < 1.f)
			precision = 3;

		printf("%ld bytes from ", sizeof(t_icmp));
		if (g_params.hostname)
			printf("%s (%s)", g_params.hostname, address);
		else
			printf("%s", address);
		printf(": icmp_seq=%d ttl=%d time=%.*f ms\n",
			req->icmp_sequence,
			reply->ip_header.ttl,
			precision, req->elapsed_time
		);
	}
	else if (req->state == DUPLICATE)
		printf("%d was duplicated\n", req->icmp_sequence);
	else if (req->state == CORRUPTED)
		printf("%d was corrupted\n", req->icmp_sequence);
}
