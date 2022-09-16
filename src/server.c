/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/11 14:29:25 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/15 02:10:39 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

int main()
{
	char buffer[1000];
	int sock;
	int n;

	sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sock == -1)
		return 1;

	int count = 0;

	printf("waiting...\n");
	while (count >= 0)
	{
		n = recvfrom(sock, buffer, 1000, 0, NULL, NULL);
		printf("received %d bytes:\n", n);

    	struct iphdr *ip_hdr = (struct iphdr *)buffer;
    	printf("IP header is %d bytes.\n", ip_hdr->ihl*4);

		printf("Version: %d\n", ip_hdr->version);
		printf("IHL: %d\n", ip_hdr->ihl * 4);
		printf("Type of Service: %d\n", ip_hdr->tos);
		printf("Total length: %d\n", htons(ip_hdr->tot_len));
		printf("Identication: %d\n", ip_hdr->id);
		printf("Fragment offset: %d\n", ip_hdr->frag_off);
		printf("Time to live: %d\n", ip_hdr->ttl);
		printf("Protocol: %d\n", ip_hdr->protocol);
		printf("Header checksum: %d\n", ip_hdr->check);
		printf("Source address: %d\n", ip_hdr->saddr);
		printf("Destination address: %d\n", ip_hdr->daddr);

    	for (int i = 0; i < n; i++) {
    	  printf("%02X%s", (uint8_t)buffer[i], (i + 1)%16 ? " " : "\n");
    	}
    	printf("\n");

    	struct icmphdr *icmp_hdr = (struct icmphdr *)((char *)ip_hdr + (4 * ip_hdr->ihl));
    	printf("ICMP msgtype=%d, code=%d\n", icmp_hdr->type, icmp_hdr->code);
    	printf("ICMP checksum=%d\n", icmp_hdr->checksum);
    	printf("ICMP id=%d, seq=%d\n\n", icmp_hdr->un.echo.id, icmp_hdr->un.echo.sequence);

		count++;
	}

	close(sock);
	return 0;
}
