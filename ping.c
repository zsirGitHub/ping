#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define uchar unsigned char
#define ushort unsigned short
#define uint unsigned int 

typedef struct icmphdr_t{
    uchar    type;
    uchar    code;
    ushort    cksum;
    ushort    id;
    ushort    seq_num;
    uint    data;
}icmphdr_t;

// It is part from iputil's ping.c
unsigned short in_cksum(unsigned short *addr, int len){
    register int sum = 0;
    u_short answer = 0;
    register u_short *w = addr;
    register int nleft = len;
    while (nleft > 1){
        sum += *w++;
        nleft -= 2;
    }
    if (nleft == 1){
        *(u_char *) (&answer) = *(u_char *) w;
        sum += answer;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return (answer);
}

int main(int argc, char *argv[])
{
    int sockfd;
    icmphdr_t *picmp_pkt;
    struct sockaddr_in des_addr, recv_addr;
    uchar data[sizeof(icmphdr_t) + 32];
    uchar recv_buf[1024];
    int len;
    int ret;
    
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(sockfd < 0) {
        printf("create socket failed!!\n");
        return -1;
    }

    memset(data, 0, sizeof(icmphdr_t) + 32);
    picmp_pkt = (icmphdr_t *)data;
    picmp_pkt->type = 8;
    picmp_pkt->code = 0;
    picmp_pkt->cksum = 0;
    picmp_pkt->id = 0;
    picmp_pkt->seq_num = 0;
    picmp_pkt->data = 0;

    picmp_pkt->cksum = in_cksum((ushort *)data, sizeof(icmphdr_t) + 32); //must be set
    
    des_addr.sin_family = AF_INET;
    des_addr.sin_addr.s_addr = inet_addr("192.168.1.1");
    des_addr.sin_port = htons(0);
    ret = sendto(sockfd, data, sizeof(icmphdr_t) + 32, 0, (struct sockaddr *)&des_addr, sizeof(des_addr));
    if(ret < 0) {
        printf("rsendto failed!!\n");
        return -1;
    }

    len = sizeof(recv_addr);
    ret = recvfrom(sockfd, recv_buf, 1024, 0, (struct sockaddr *)&recv_addr, &len);
    if(ret < 0) {
        printf("recvfrom failed!!\n");
        return -1;
    }

    printf("ping success receive data from:%s\n", inet_ntoa(recv_addr.sin_addr));

    return 0;
}
