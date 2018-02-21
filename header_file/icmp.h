#ifndef __ICMP_H
#define __ICMP_H

#define ICMP_TYPE_REPLY		0		//����Ӧ������Ϊ0
#define ICMP_TYPE_REQUEST	8		//������ԣ�����Ϊ8

struct icmp_hdr
{
  //����+����涨�˸�icmp��������ʵ��ʲô����
  unsigned char type;
  unsigned char code;
  unsigned short checksum;
  //��ʶ��
  unsigned short id;
  //���к�
  unsigned short sequence;
  unsigned char padding[];
} __attribute__((packed));

unsigned short icmp_compute_checksum(void *data, unsigned short len);
void icmp_send_reply(unsigned char *to, unsigned char *payload,
		     unsigned short len, unsigned short id, unsigned short sequence);
void icmp_handle(unsigned char *src_ip, unsigned char *data, unsigned short len);

#endif