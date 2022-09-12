/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/11 14:29:25 by mamartin          #+#    #+#             */
/*   Updated: 2022/09/12 01:08:33 by mamartin         ###   ########.fr       */
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
		struct msghdr msg;
		struct iovec iov;
		char addrbuf[128];
		char ans_data[4096];

		memset(&msg, 0, sizeof(msg));

		iov.iov_len = 84;
		iov.iov_base = malloc(84);
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
		// msg.msg_name = addrbuf;
		// msg.msg_namelen = sizeof(addrbuf);
		// msg.msg_control = ans_data;
		// msg.msg_controllen = sizeof(ans_data);
		// msg.msg_flags = 0;

		n = recvmsg(sock, &msg, MSG_TRUNC);
		if (n == -1)
		{
			printf("%d\n", errno);
			perror("recvmsg");
		}

		// n = recvfrom(sock, buffer, 1000, 0, NULL, NULL);
		printf("received %d bytes: %d\n", n, msg.msg_flags);

		struct cmsghdr* ptr = CMSG_FIRSTHDR(&msg);
		// printf("%d\n", msg.msg_controllen);
		// printf("%d\n", msg.msg_namelen);
		// printf("%d\n", msg.msg_iovlen);

		struct timeval* tv = (struct timeval*)(char*)(iov.iov_base + 28);
		printf("%d %d\n", tv->tv_sec, tv->tv_usec);
		// printf("%d\n", ptr->cmsg_level);
		// printf("%d\n", ptr->cmsg_type);
		// printf("%d\n\n", ptr->cmsg_len);

    	// struct iphdr *ip_hdr = (struct iphdr *)buffer;
    	// printf("IP header is %d bytes.\n", ip_hdr->ihl*4);

		// printf("Version: %d\n", ip_hdr->version);
		// printf("IHL: %d\n", ip_hdr->ihl * 4);
		// printf("Type of Service: %d\n", ip_hdr->tos);
		// printf("Total length: %d\n", htons(ip_hdr->tot_len));
		// printf("Identication: %d\n", ip_hdr->id);
		// printf("Fragment offset: %d\n", ip_hdr->frag_off);
		// printf("Time to live: %d\n", ip_hdr->ttl);
		// printf("Protocol: %d\n", ip_hdr->protocol);
		// printf("Header checksum: %d\n", ip_hdr->check);
		// printf("Source address: %d\n", ip_hdr->saddr);
		// printf("Destination address: %d\n", ip_hdr->daddr);

    	// for (int i = 0; i < n; i++) {
    	//   printf("%02X%s", (uint8_t)buffer[i], (i + 1)%16 ? " " : "\n");
    	// }
    	// printf("\n");

    	// struct icmphdr *icmp_hdr = (struct icmphdr *)((char *)ip_hdr + (4 * ip_hdr->ihl));
    	// printf("ICMP msgtype=%d, code=%d\n", icmp_hdr->type, icmp_hdr->code);
    	// printf("ICMP checksum=%d\n", icmp_hdr->checksum);
    	// printf("ICMP id=%d, seq=%d\n\n", icmp_hdr->un.echo.id, icmp_hdr->un.echo.sequence);

		count++;
	}

	close(sock);
	return 0;
}
