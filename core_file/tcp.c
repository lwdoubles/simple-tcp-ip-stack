#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<apra/inet.h>

#include "ip.h"
#include "tcp.h"

char *tcp_state_check(enum tcp_state state)
{
	switch (state)
    {
		case LISTEN:
			return "LISTEN";
		case SYN_SENT:
			return "SYN_SENT";
		case SYN_RECEIVED:
			return "SYN_RECEIVED";
		case ESTABLISHED:
			return "ESTABLISHED";
		case FIN_WAIT_1:
			return "FIN_WAIT_1";
		case FIN_WAIT_2:
			return "FIN_WAIT_2";
		case CLOSE_WAIT:
			return "CLOSE_WAIT";
		case CLOSING:
			return "CLOSING";
		case LAST_ACK:
			return "LAST_ACK";
		case TIME_WAIT:
			return "TIME_WAIT";
		case CLOSED:
			return "CLOSED";
		default:
			return "error";
	}
}


