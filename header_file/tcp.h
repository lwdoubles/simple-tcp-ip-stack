#ifndef	__TCP_H
#define __TCP_H

#ifdef __cplusplus
extern "C"{
#endif

//TCP状态
enum tcp_state
{
	LISTEN,
    SYN_SENT,
    SYN_RECEIVED,
    ESTABLISHED,
    FIN_WAIT_1,
    FIN_WAIT_2,
    CLOSE_WAIT,
    CLOSING,		//这个状态似乎是用于同时关闭的
    LAST_ACK,
    TIME_WAIT,
    CLOSED
};

//TCP控制块
struct tcp_tcb
{
	unsigned short local_port;	
	unsigned char remote_ip[4];
	unsigned short remote_port;
	unsigned long snd_una;
	unsigned long snd_nxt;
	unsigned short snd_wnd;
	unsigned short snd_up;
	unsigned short snd_wl1;
	unsigned short snd_wl2;
	unsigned long iss;
	unsigned long rcv_nxt;
	unsigned short rcv_wnd;
	unsigned short rcv_up;
	unsigned long irs;
	unsigned long seg_seq;
	unsigned long seg_ack;
	unsigned short seg_len;
	unsigned short seg_wnd;
	unsigned short seg_up;
	unsigned short seg_prc;
	enum tcp_sock_state state;
	int passive;
}

//TCP头部
struct tcphdr
{
	usigned short src_port;
	usigned short dst_port;
	usigned long seq_num;
	usigned long ack_num;
	//这里数据偏移暂时不写,似乎缺一个大小端判断
	usigned char _flags[];		//ACK.FON,PSH,RST,SYN,FIN
#define tcp_fin    0x01
#define tcp_syn    0x02
#define tcp_rst    0x04
#define tcp_push   0x08
#define tcp_ack    0x10
#define tcp_urg    0x20
	usigned short ack_window;
	usigned short tcp_check_sum;
	usigned short urgent_offset;
	
	//暂时未加入选项


	
};

void tcp_init();

int tcp_open(int passive, unsigned short local_port,
	     unsigned char remote_ip[], unsigned short remote_port);
void tcp_send(int sd, const char *buf, size_t sz, int psh, int urg);
size_t tcp_recv(int sd, char *buf, size_t sz);
void tcp_close(int sd);

struct tcp_tcb *tcp_status(int sd);
void tcp_abort(int sd);	

unsigned short tcp_compute_checksum(void *data, unsigned short len);
void tcp_handle(unsigned char *src_ip, unsigned char *dest_ip, unsigned char *data, unsigned short len);
void tcp_dump_tcb(void);

#ifdef __cplusplus
}
#endif

#endif
