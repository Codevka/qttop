#include "network.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/sockios.h> /* for net status mask */
#include <net/if.h>        /* for ifconf */
#include <net/if_arp.h>
#include <netinet/in.h> /* for sockaddr_in */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define CMD \
    "/bin/cat /proc/net/dev |awk \'{ if(NR!=1 && NR!=2) print $0 }\' |sed 's/:/ /g'| awk \'{ " \
    "print $1\" \"$2\" \"$3\" \"$5\" \"$10\" \"$11\" \"$13}\'"
#define MAX_INTERFACE (16)

void getnetflowinfo(netflowinfo &nfi)
{
    FILE *fp = popen(CMD, "r");
    if (fp == nullptr) {
        return;
    }

    netflowinfo stTmp;
    while (fscanf(fp,
                  "%s%" PRIu64 "%" PRIu64 "%u%" PRIu64 "%" PRIu64 "%u",
                  stTmp.netname,
                  &stTmp.recvbytes,
                  &stTmp.recvpackets,
                  &stTmp.recvdroppackets,
                  &stTmp.sendbytes,
                  &stTmp.sendpackets,
                  &stTmp.senddroppackets)
           == 7) {
        if (strstr(stTmp.netname, nfi.netname)) {
            memcpy(&nfi, &stTmp, sizeof(stTmp));
        }
    }
    pclose(fp);
}

int getnetinfo(netinfo &ni)
{
    int fd;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd > 0) {
        struct ifreq buf[MAX_INTERFACE];
        struct ifconf ifc;
        int ret = 0;
        int if_num = 0;
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = (caddr_t) buf;

        ret = ioctl(fd, SIOCGIFCONF, (char *) &ifc);
        if (ret) {
            printf("get if config info failed");
            return -1;
        }
        if_num = ifc.ifc_len / sizeof(struct ifreq);

        while (if_num-- > 0) {
            printf("net device: %s\n", buf[if_num].ifr_name);
            ni.netname << buf[if_num].ifr_name;
            /* 获取第n个网口信息 */
            ret = ioctl(fd, SIOCGIFFLAGS, (char *) &buf[if_num]);
            if (ret)
                continue;

            /* ip地址 */
            ret = ioctl(fd, SIOCGIFADDR, (char *) &buf[if_num]);
            if (ret)
                continue;
            printf("IP address is: \n%s\n",
                   inet_ntoa(((struct sockaddr_in *) (&buf[if_num].ifr_addr))->sin_addr));
            ni.ipadd << inet_ntoa(((struct sockaddr_in *) (&buf[if_num].ifr_addr))->sin_addr);

            /* mac */
            ret = ioctl(fd, SIOCGIFHWADDR, (char *) &buf[if_num]);
            if (ret)
                continue;

            printf("%02x:%02x:%02x:%02x:%02x:%02x\n\n",
                   (unsigned char) buf[if_num].ifr_hwaddr.sa_data[0],
                   (unsigned char) buf[if_num].ifr_hwaddr.sa_data[1],
                   (unsigned char) buf[if_num].ifr_hwaddr.sa_data[2],
                   (unsigned char) buf[if_num].ifr_hwaddr.sa_data[3],
                   (unsigned char) buf[if_num].ifr_hwaddr.sa_data[4],
                   (unsigned char) buf[if_num].ifr_hwaddr.sa_data[5]);
            close(fd);
        }
    }
    return 0;
}
