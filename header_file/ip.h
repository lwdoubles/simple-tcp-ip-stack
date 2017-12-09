#ifndef __IP_H
#define  __IP_H

struct ip_hdr
{
  
  //先默认为小端模式
  unsigned int header_len:4;
  unsigned int ip_version:4;

  unsigned char ip_tos;			//服务类型，没什么用的
  unsigned short total_len;
  unsigned short id;			//分片时才有用
  unsigned short frag_offset;	//片偏移，分片时才有用，某片在原分组的相对位置
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