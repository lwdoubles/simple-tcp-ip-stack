#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include "ether.h"
#include "arp.h"
#include "ip.h"

/*- TEMPORARY -*/
extern unsigned char mac_address[];
/*-------------*/

struct arp_set arp_sets[256];

//初始化硬件地址
void arp_init(void)
{
  int i;

  for (i = 0; i < 256; ++i)
    arp_sets[i].present = 0;
}

//打印IP和硬件地址的对应关系，相当于arp高速缓存
void arp_set_dump(void)
{
  int i;

#ifdef DEBUG_ARP
  printf("--- ARP SET ---\n");
  for (i = 0; i < 256; ++i)
    {
      if (arp_sets[i].present)
		{
			printf("%d.%d.%d.%d %02X:%02X:%02X:%02X:%02X:%02X\n",
			arp_sets[i].ip_addr[0],
			arp_sets[i].ip_addr[1],
			arp_sets[i].ip_addr[2],
			arp_sets[i].ip_addr[3],
			arp_sets[i].mac_addr[0],
			arp_sets[i].mac_addr[1],
			arp_sets[i].mac_addr[2],
			arp_sets[i].mac_addr[3],
			arp_sets[i].mac_addr[4],
			arp_sets[i].mac_addr[5]);
		}
    }  
  printf("----------------------------------\n");
#endif /* DEBUG_ARP */
}


//更新arp高速缓存
void arp_set_add(unsigned char *ip_addr, unsigned char *mac_addr)
{
  int i;
  int last_available = -1;
  int oldest = -1;

  for (i = 0; i < 256; ++i)
  {
		if (arp_sets[i].present)
		{
			if (memcmp(ip_addr, arp_sets[i].ip_addr, 4) == 0)
			{
				memcpy(arp_sets[i].mac_addr, mac_addr, 6);
				arp_sets[i].age = time(NULL);
				return;
			}
			if (oldest == -1)
				oldest = i;
			else
				if (arp_sets[i].age < arp_sets[oldest].age)
					oldest = i;
		}
		else
			last_available = i;
  }

  if (last_available != -1)
  {
      memcpy(arp_sets[last_available].mac_addr, mac_addr, 6);
      memcpy(arp_sets[last_available].ip_addr, ip_addr, 4);
      arp_sets[last_available].present = 1;
      arp_sets[last_available].age = time(NULL);
  }
  else
  {
      memcpy(arp_sets[oldest].mac_addr, mac_addr, 6);
      memcpy(arp_sets[oldest].ip_addr, ip_addr, 4);
      arp_sets[oldest].age = time(NULL);
  }

  //最后打印出arp高速缓存
  arp_set_dump();
}

//arp查找，查找到的硬件地址由dest_arp返回
void arp_resolve(unsigned char *to, unsigned char *dest_arp)
{
  int i;

  for (i = 0; i < 256; ++i)
  {
      if (arp_sets[i].present)
	  {
		  if (memcmp(to, arp_sets[i].ip_addr, 4) == 0)
		  {	
			  memcpy(dest_arp, arp_sets[i].mac_addr, 6);
			  break;
		  }
	  }
  }


#ifdef DEBUG_ARP
  if (i == 256)
    {
      printf("ARP SET NOT FOUND !\n");
    }
#endif /* DEBUG_ARP */
}

//发送arp应答
void arp_send_reply(unsigned char *to, unsigned char *sha,
		    unsigned char *spa, unsigned char *tpa)
{
	struct arp_hdr data;

	data.hdware_type = htons(1);	//表示以太网请求
	data.protol_type = htons(ETHER_TYPE_IPV4);
	data.hdware_len = 6;
	data.protol_len = 4;
	data.op = htons(ARP_OPER_REPLY);
	memcpy(data.sha, mac_address, 6);
	memcpy(data.spa, tpa, 4);
	memcpy(data.tha, sha, 6);
	memcpy(data.tpa, spa, 4);
	ether_send_frame(to, ETHER_TYPE_ARP, (unsigned char *)&data, sizeof(data));
}

//处理arp报文
void arp_handle(unsigned char *src_mac, unsigned char *data, unsigned short len)
{
  struct arp_hdr *pkt = (struct arp_hdr *)data;

  switch (ntohs(pkt->op))
    {
	//处理arp请求报文
    case ARP_OPER_REQUEST:
#ifdef DEBUG_ARP
      printf("ARP Request for %d.%d.%d.%d\n",
	     pkt->tpa[0], pkt->tpa[1],
	     pkt->tpa[2], pkt->tpa[3]);
#endif /* DEBUG_ARP */
      arp_table_add(pkt->spa, pkt->sha);
      if (memcmp(pkt->tpa, ip_address, 4) == 0)
	arp_send_reply(src_mac, pkt->sha, pkt->spa, pkt->tpa);
      break;

	  //处理arp回应报文
    case ARP_OPER_REPLY:
#ifdef DEBUG_ARP
      printf("ARP Reply\n");
#endif /* DEBUG_ARP */
      arp_table_add(pkt->spa, pkt->sha);
      break;
    default:
      break;
    }
}


/*- TEMPORARY -*/
extern int tap_fd;
/*-------------*/

unsigned char mac_address[6] = {0x00, 0xff, 0x42, 0x12, 0x34, 0x56};
unsigned char bcast_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

//发送以太网帧
void ether_send_frame(unsigned char *to, unsigned short type,
		      unsigned char *payload, unsigned short size)
{
	unsigned char raw[sizeof(struct ether_hdr) + size];
	struct ether_hdr *data = (struct ether_hdr *)&raw;

#ifdef DEBUG_ETHER
	printf("Sending size %d\n", size);
#endif /* DEBUG_ETHER */

	memcpy(data->src_mac, mac_address, 6);
	memcpy(data->dst_mac, to, 6);
	data->type = htons(type);
#ifdef DEBUG_ETHER
	printf("Send 0x%04x 0x%04x\n", type, data->type);
#endif /* DEBUG_ETHER */
	memcpy(data->payload, payload, size);
	write(tap_fd, data, sizeof(struct ether_hdr) + size);
}

//接收以太网帧
void ether_recv_frame(void)
{
	int len;
	char buf[1522];
	struct ether_hdr *ether_data = (struct ether_hdr *)buf;

	len = read(tap_fd, buf, 1522);
	if (len <= 0)
		return;

#ifdef DEBUG_ETHER
	printf("%02x:%02x:%02x:%02x:%02x:%02x -> %02x:%02x:%02x:%02x:%02x:%02x protocol 0x%04x\n",
		ether_data->src_mac[0], ether_data->src_mac[1],
		ether_data->src_mac[2], ether_data->src_mac[3],
		ether_data->src_mac[4], ether_data->src_mac[5],
		ether_data->dst_mac[0], ether_data->dst_mac[1],
		ether_data->dst_mac[2], ether_data->dst_mac[3],
		ether_data->dst_mac[4], ether_data->dst_mac[5],
	    ntohs(ether_data->type));
#endif /* DEBUG_ETHER */
  if(memcmp(ether_data->dst_mac, mac_address, 6) == 0 ||
      memcmp(ether_data->dst_mac, bcast_mac, 6) == 0)
    {
#ifdef DEBUG_ETHER
      printf("This one is for me\n");
#endif /* DEBUG_ETHER */
      switch (ntohs(ether_data->type))
	{
	case ETHER_TYPE_ARP:
	  arp_handle(ether_data->src_mac, ether_data->payload, len);
	  break;
	case ETHER_TYPE_IPV4:
	  ip_handle(ether_data->src_mac, ether_data->payload, len);
	  break;
	default:
	  break;
	}
    }
}

//检测tap_fd是否可读
int ether_can_recv()
{
	fd_set fds;
	struct timeval tv;

	FD_ZERO(&fds);
	FD_SET(tap_fd, &fds);
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	return select(tap_fd + 1, &fds, NULL, NULL, &tv);
}
