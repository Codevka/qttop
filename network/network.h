#ifndef _net_info_h_
#define _net_info_h_

#include<inttypes.h>
#include<QStringList>
typedef struct
{
    char netname[32];
    uint64_t recvpackets;
    uint64_t sendpackets;
    uint64_t recvbytes;
    uint64_t sendbytes;
    uint32_t recvdroppackets;
    uint32_t senddroppackets;
}netflowinfo;

typedef struct
{
    QStringList netname;
    QStringList ipadd;

    
}netinfo;

void getnetflowinfo(netflowinfo &nfi);
int getnetinfo(netinfo &ni);
#endif
