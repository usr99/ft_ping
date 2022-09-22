/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/10 16:48:55 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/22 15:48:52 by mamartin         ###   ########.fr       */
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
	t_icmp_msg icmpmsg = { 0 };
	t_ping_request* req;
	int icmp_type;
	int replies_received = 0;

	g_params.program_name = argv[0];
	destination_addr = parse_arguments(argc, argv);
	if (getuid() != 0)
		exit_error("user must have root privileges !");

	if (signal(SIGALRM, send_ping) == SIG_ERR)
		exit_error("failed to set SIGALRM handling behavior");
	if (signal(SIGINT, sigint_handler) == SIG_ERR)
		exit_error("failed to set SIGINT handling behavior");
	if (signal(SIGQUIT, print_stats_sigquit) == SIG_ERR)
		exit_error("failed to set SIGQUIT handling behavior");

	init_ping(&g_params, destination_addr, addrname);

	gettimeofday(&start, NULL);
	send_ping(SIGALRM);
	while (!g_params.finished)
	{
		icmp_type = recv_icmp_message(g_params.sockfd, &icmpmsg);
		if (icmp_type != -1)
		{
			replies_received++;
			if (replies_received == g_params.options.count)
				g_params.finished = 1;

			req = update_request(&icmpmsg);
			if (g_params.options.quiet != 1)
			{
				if (icmp_type == ICMP_ECHOREPLY)
					log_reply(&icmpmsg, req, addrname);
				else if (g_params.options.verbose)
					log_error(&icmpmsg);
			}
			free(icmpmsg.ip);
		}

		if (g_params.options.deadline != -1 && get_duration_ms(start) / 1000 > g_params.options.deadline)
			g_params.finished = 1;
	}

	print_statistics(g_params.requests, destination_addr, start);
	clean_all();
	return replies_received ? 0 : 1;
}

void init_ping(t_ping_params* params, const char* destination, char* addrname)
{
	struct addrinfo* results;
	struct addrinfo hint;

	/* Create a raw socket for ICMP protocol */
	params->sockfd = socket(AF_INET, SOCK_RAW | SOCK_NONBLOCK, IPPROTO_ICMP);
	if (params->sockfd == -1)
		exit_error("Socket creation failed");

	/* Set user-defined ttl */
	if (params->options.ttl != -1)
	{
		if (setsockopt(params->sockfd, IPPROTO_IP, IP_TTL, &params->options.ttl, sizeof(int16_t)) != 0)
			exit_error("Failed to set TTL value");
	}

	/* DNS lookup to find the address under the domain name */
	ft_memset(&hint, 0, sizeof(struct addrinfo));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_RAW;
	hint.ai_protocol = IPPROTO_ICMP;
	hint.ai_flags = AI_CANONNAME;
	if (getaddrinfo(destination, NULL, &hint, &results) != 0)
		exit_error("Name or service not known");

	/* Copy address and hostname before freeing the results */
	params->address = malloc(sizeof(struct sockaddr_in));
	if (!params->address)
		exit_error("alloc failed");
	if (results->ai_addrlen != sizeof(struct sockaddr_in))
		exit_error("bad address structure");
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
			exit_error("failed to convert address into a string format");
		/* Only set hostname if it's not equal to the ip address */
		if (ft_strncmp(results->ai_canonname, addrname, INET_ADDRSTRLEN) != 0)
		{
			params->hostname = ft_strdup(results->ai_canonname);
			if (!params->hostname)
				exit_error("alloc failed");
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
	if (g_params.finished)
		return ;

	t_icmp_echo packet = create_echo_message(g_params.pid, g_params.icmp_count);
	if (sendto(g_params.sockfd, &packet, sizeof(t_icmp_echo), 0, (struct sockaddr*)g_params.address, sizeof(struct sockaddr_in)) == -1)
		exit_error("failed to send message");

	if (!push_new_node(&g_params.requests, g_params.icmp_count))
		exit_error("alloc failed");

	g_params.icmp_count++;
	if (g_params.options.count != -1 && g_params.icmp_count > g_params.options.count)
		alarm(0);
	else
	{
		if (g_params.icmp_count <= g_params.options.preload)
			send_ping(signum);
		else
			alarm(g_params.finished ? 0 : 1);
	}
}

t_ping_request* update_request(t_icmp_msg* message)
{
	t_list* node;
	t_ping_request* req;
	uint16_t sequence;

	if (message->icmp->type == ICMP_ECHOREPLY)
		sequence = message->icmp->un.echo.sequence;
	else
		sequence = ((t_icmp_echo*)((char*)message->payload + IPHEADER_SIZE))->header.un.echo.sequence;

	node = get_stat(g_params.requests, sequence);
	if (!node)
		exit_error("unexpected error"); // should never happen
	req = (t_ping_request*)node->content;

	if (req->state != WAITING_REPLY)
	{
		/* Duplicate the ping request for further rtt calculations */
		t_ping_request *duplicate;
		duplicate = push_new_node(&g_params.requests, req->icmp_sequence);
		if (!duplicate)
			exit_error("alloc failed");
		duplicate->elapsed_time = get_duration_ms(*(struct timeval*)message->payload);
		duplicate->state = DUPLICATE;
		return duplicate;
	}

	if (message->status == BAD_CHECKSUM)
		req->state = CORRUPTED;
	else
	{
		if (message->icmp->type != ICMP_ECHOREPLY)
			req->state = ICMP_ERR;
		else
		{
			req->state = SUCCESS;
			req->elapsed_time = get_duration_ms(*(struct timeval*)message->payload);
		}
	}

	return req;
}

void log_reply(t_icmp_msg* reply, t_ping_request* req, const char* address)
{
	const char* addrname;
	char hostname[1024];
	int precision = 1;

	if (reverse_dns_lookup(hostname, 1024, reply->ip->saddr) != 0)
		addrname = g_params.hostname;
	else
		addrname = hostname;

	if (reply->status == OK)
	{
		if (req->elapsed_time < 1.f)
			precision = 3;

		printf("%ld bytes from ", sizeof(t_icmp_echo));
		if (addrname && g_params.options.numeric_output == 0)
			printf("%s (%s)", addrname, address);
		else
			printf("%s", address);
		printf(": icmp_seq=%d ttl=%d time=%.*f ms\n",
			req->icmp_sequence,
			reply->ip->ttl,
			precision, req->elapsed_time
		);
	}
	else
	{
		printf("From %s: icmp_seq=%d BAD CHECKSUM",
			(addrname && g_params.options.numeric_output == 0) ? addrname : address,
			req->icmp_sequence
		);
	}
}

void log_error(t_icmp_msg* err)
{
	static const char* error_messages[] = {
		"Destination Unreachable",
		"Source Quench",
		"Redirect",
		"Time Exceeded",
		"Parameter Problem",
		"Timestamp",
		"Timestamp Reply",
		"Information Request",
		"Information Reply"
	};
	static const uint8_t errtypes[] = {
		3, 4, 5, 11, 12, 13, 14, 15, 16
	};

	const char* addrname;
	char hostname[1024];

	unsigned int idx;
	char addr[INET_ADDRSTRLEN] = { 0 };
	struct in_addr src;

	if (reverse_dns_lookup(hostname, 1024, err->ip->saddr) != 0)
		addrname = g_params.hostname;
	else
		addrname = hostname;

	for (
		idx = 0;
		idx < sizeof(errtypes) && errtypes[idx] != err->icmp->type;
		idx++
	);

	src.s_addr = err->ip->saddr;
	if (!inet_ntop(AF_INET, &src, addr, INET_ADDRSTRLEN))
		exit_error("failed to convert address into a string format");

	if (addrname && g_params.options.numeric_output == 0)
		printf("From %s (%s) ", addrname, addr);
	else
		printf("From %s ", addr);
	printf("icmp_seq=%d %s\n",
		((t_icmp_echo*)((char*)err->payload + IPHEADER_SIZE))->header.un.echo.sequence,
		error_messages[idx]
	);
}

int reverse_dns_lookup(char* hostname, size_t buflen, in_addr_t address)
{
	struct sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = address;

	return getnameinfo(
		(struct sockaddr*)&addr, sizeof(struct sockaddr_in),
		hostname, buflen,
		NULL, 0, 0
	);
}
