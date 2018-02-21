#ifndef __ICMP_H
#define __ICMP_H

#define ICMP_TYPE_REPLY		0		//回显应答，类型为0
#define ICMP_TYPE_REQUEST	8		//请求回显，类型为8

struct icmp_hdr
{
  //类型+代码规定了该icmp报文用于实现什么功能
  unsigned char type;
  unsigned char code;
  unsigned short checksum;
  //标识符
  unsigned short id;
  //序列号
  unsigned short sequence;
  unsigned char padding[];
} __attribute__((packed));

unsigned short icmp_compute_checksum(void *data, unsigned short len);
void icmp_send_reply(unsigned char *to, unsigned char *payload,
		     unsigned short len, unsigned short id, unsigned short sequence);
void icmp_handle(unsigned char *src_ip, unsigned char *data, unsigned short len);

#endif