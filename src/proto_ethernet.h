/*
 * netsniff-ng - the packet sniffing beast
 * By Daniel Borkmann <daniel@netsniff-ng.org>
 * Copyright 2009, 2010 Daniel Borkmann.
 * Subject to the GPL, version 2.
 */

#ifndef PROTO_ETHERNET_H
#define PROTO_ETHERNET_H

#include <stdio.h>
#include <stdint.h>
#include <netinet/in.h>
#include <linux/if_ether.h>

#include "proto_struct.h"
#include "proto_none.h"
#include "dissector_eth.h"
#include "pkt_buff.h"

static inline void ethernet(struct pkt_buff *pkt)
{
	uint8_t *src_mac, *dst_mac;
	struct ethhdr *eth = (struct ethhdr *) pkt_pull(pkt, sizeof(*eth));

	if (eth == NULL)
		return;

	src_mac = eth->h_source;
	dst_mac = eth->h_dest;
	alloc_string(*pkt->buffer_pkt," [ Eth ");
	alloc_string(*pkt->buffer_pkt,"MAC (%.2x:%.2x:%.2x:%.2x:%.2x:%.2x => ",
		src_mac[0], src_mac[1], src_mac[2],
		src_mac[3], src_mac[4], src_mac[5]);
	alloc_string(*pkt->buffer_pkt,"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x), ",
		dst_mac[0], dst_mac[1], dst_mac[2],
		dst_mac[3], dst_mac[4], dst_mac[5]);
	alloc_string(*pkt->buffer_pkt,"Proto (0x%.4x, %s%s%s)",
		ntohs(eth->h_proto), colorize_start(bold), 
		lookup_ether_type(ntohs(eth->h_proto)), colorize_end());
	alloc_string(*pkt->buffer_pkt," ]\n");

	alloc_string(*pkt->buffer_pkt," [ Vendor ");
	alloc_string(*pkt->buffer_pkt,"(%s => %s)",
		lookup_vendor((src_mac[0] << 16) | (src_mac[1] << 8) |
			      src_mac[2]),
		lookup_vendor((dst_mac[0] << 16) | (dst_mac[1] << 8) |
			      dst_mac[2]));
	alloc_string(*pkt->buffer_pkt," ]\n");

	pkt_set_proto(pkt, &eth_lay2, ntohs(eth->h_proto));
}

static inline void ethernet_hex_all(struct pkt_buff *pkt)
{
	alloc_string(*pkt->buffer_pkt,"   ");
	hex(pkt);
}

static inline void ethernet_less(struct pkt_buff *pkt)
{
	uint8_t *src_mac, *dst_mac;
	struct ethhdr *eth = (struct ethhdr *) pkt_pull(pkt, sizeof(*eth));

	if (eth == NULL)
		return;

	src_mac = eth->h_source;
	dst_mac = eth->h_dest;
	alloc_string(*pkt->buffer_pkt," %s => %s ",
		lookup_vendor((src_mac[0] << 16) | (src_mac[1] << 8) |
			      src_mac[2]),
		lookup_vendor((dst_mac[0] << 16) | (dst_mac[1] << 8) |
			      dst_mac[2]));
	alloc_string(*pkt->buffer_pkt,"%s%s%s", colorize_start(bold),
		lookup_ether_type(ntohs(eth->h_proto)), colorize_end());

	pkt_set_proto(pkt, &eth_lay2, ntohs(eth->h_proto));
}

struct protocol ethernet_ops = {
	.key = 0,
	.print_full = ethernet,
	.print_less = ethernet_less,
};

#endif /* PROTO_ETHERNET_H */
