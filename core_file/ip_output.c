#ifndef __IP_OUTPUT_H
#define __IP_OUTPUT_H

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ip.h"
#include "arp.h"
#include "icmp.h"
#include "ether.h"
#include "tcp.h"

//发送IP数据报
void ip_send_packet(unsigned char *to, unsigned char proto, unsigned char tos,
		    unsigned char *payload, unsigned short len, unsigned int nofrag)
{

  //新建一个ip_hdr对象
  unsigned char raw[sizeof(struct ip_hdr) + len];
  struct ip_hdr *data = (struct ip_hdr *)&raw;
  unsigned char dest_arp[6];
  
  
#ifdef DEBUG_IP
      printf("Sending IP packet\n");
#endif /* DEBUG_IP */

  data->header_len = 5;
  data->ip_version = 4;
  data->ip_tos = tos;
  data->total_len = htons(sizeof(raw));
  data->id = htons(ip_id++);
  data->frag_offset = htons(!!nofrag ? 0x4000 : 0);
  data->ip_ttl = 42;
  data->ip_protol = proto;
  data->ip_check_sum = 0;
  memcpy(data->src_addr, ip_address, 4);
  memcpy(data->dst_addr, to, 4);
  memcpy(data->opt, payload, len);
  data->chksum = ip_compute_checksum(raw, sizeof(struct ip_hdr));
  arp_resolve(to, dest_arp);
  ether_send_frame(dest_arp, ETHER_TYPE_IPV4, raw, sizeof(raw));
}





#endif