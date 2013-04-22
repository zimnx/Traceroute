#ifndef __ICMP_H
#define __ICMP_H

#define ICMP_HEADER_LEN 8

short in_cksum( short *addr, register int len, short csum);

#endif
