#ifndef __IP_H
#define  __IP_H

struct ip_hdr
{
  
  //��Ĭ��ΪС��ģʽ
  unsigned int header_len:4;
  unsigned int ip_version:4;

  unsigned char ip_tos;			//�������ͣ�ûʲô�õ�
  unsigned short total_len;
  unsigned short id;			//��Ƭʱ������
  unsigned short frag_offset;	//Ƭƫ�ƣ���Ƭʱ�����ã�ĳƬ��ԭ��������λ��
  unsigned char ttl;			
  unsigned char ip_protol;
  unsigned short ip_check_sum;
  unsigned char src_addr[4];
  unsigned char dst_addr[4];
  unsigned char opt[];

}

void ip_init(void);

unsigned short ip_checksum(void *data, unsigned short len);

void ip_handle(unsigned char *src_mac, unsigned char *data, unsigned short len);


#endif