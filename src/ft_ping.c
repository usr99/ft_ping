/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/10 16:48:55 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/10 19:02:43 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netdb.h>
#include <stdio.h>

#include "ft_ping.h"
#include "libft.h"

int main(int argc, char **argv)
{
	int sock;
	struct addrinfo hint;
	struct addrinfo* results;
	struct addrinfo* addr;

	if (argc != 2)
		exit_error("no ip provided\n");

	ft_memset(&hint, 0, sizeof(struct addrinfo));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_RAW;
	hint.ai_protocol = IPPROTO_ICMP;
	if (getaddrinfo(argv[1], NULL, &hint, &results) != 0)
		exit_error("failed to resolve host\n");

	for (addr = results; addr != NULL; addr = addr->ai_next)
	{
		sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (sock != -1)
		{
			if (bind(sock, addr->ai_addr, addr->ai_addrlen) == 0)
				break ;
			perror("bind");
			close(sock);
		}
		else
			perror("socket");
	}
	freeaddrinfo(addr);

	if (addr == NULL)
		exit_error("failed to bind\n");

	close(sock);
	return 0;
}
