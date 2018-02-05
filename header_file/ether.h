#ifndef __ETHER_H_
#define __ETHER_H_

#include <time.h>

#define ETHER_TYPE_IPV4		0x0800
#define ETHER_TYPE_ARP		0x0806

#define ARP_OPER_REQUEST	1
#define ARP_OPER_REPLY		2

struct ether_hdr
{
  unsigned char dst_mac[6];
  unsigned char src_mac[6];
  unsigned short ether_type;
  //Я������
  unsigned char payload[];
} __attribute__((packed));


struct arp_hdr
{
  unsigned short hdware_type;		//Ӳ�����ͣ�ֵΪ1��ʾ��̫��
  unsigned short protol_type;
  unsigned char  hdware_len;
  unsigned char  protol_len;
  unsigned short op;				//�����ֶΣ�Ϊ1��ʾarp����2��ʾarpΪarp��Ӧ
  unsigned char  sha[6];		//sender hardware address 
  unsigned char  spa[4];		//sender protocol address
  unsigned char  tha[6];		//target hardware address
  unsigned char  tpa[4];		//target protocol address
} __attribute__((packed));


//arp���ϣ��Լ���������
struct arp_set
{
  unsigned char ip_addr[4];
  unsigned char mac_addr[6];
  time_t age;
  int present;
};

void ether_send_frame(unsigned char *to, unsigned short type,
		      unsigned char *payload, unsigned short size);

void ether_recv_frame(void);

int ether_can_recv();

void arp_init(void);

void arp_table_dump(void);

void arp_table_add(unsigned char *ip_addr, unsigned char *mac_addr);

void arp_resolve(unsigned char *to, unsigned char *dest_arp);

void arp_send_reply(unsigned char *to, unsigned char *sha,
		    unsigned char *spa, unsigned char *tpa);

void arp_handle(unsigned char *src_mac, unsigned char *data,
		unsigned short len);

#endif /* ETHER_H_ */