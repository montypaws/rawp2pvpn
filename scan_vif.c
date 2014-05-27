#include <arpa/inet.h>
#include <linux/ip.h>
#include <netinet/if_ether.h>
#include <netinet/ether.h>
#include <net/ethernet.h>
#include "globals.h"
#include "tea.c"

unsigned char *CreateIPHeader(int ,unsigned char *);
unsigned char* CreateEthernetHdr(unsigned char *src_mac,unsigned char *dst_mac, int protocol);

typedef unsigned short u16;
typedef unsigned long u32;

ParseVirtual(unsigned char *packet, int len,int ext_sock)
{
	struct iphdr *ip_headr;
        unsigned char *enpacket=NULL;
        unsigned char *final_packet=NULL;
	unsigned char *ethernet_hdr=NULL;
	unsigned char *final_ip_hdr=NULL;
        unsigned char d_ip[16];
	int final_len,i=0;
        int enlen=len;
        char mykey[16]="secretkey";
        unsigned char *str=NULL;
        unsigned char *d_vip=NULL;
        
        str = (unsigned char*)calloc(16, sizeof(unsigned char));
        d_vip = (unsigned char*)calloc(16, sizeof(unsigned char));
        
        
        enpacket = (unsigned char*)calloc(len, sizeof(unsigned char));

        if (enpacket == NULL)
        {
            puts("Memory allocation failed.\n");
            return 0;
        }
        memcpy(enpacket,packet,len);
        
        ip_headr = (struct iphdr*)packet;
        
        d_vip = inet_ntoa(*(struct in_addr *)&ip_headr->daddr);
        
        for(i=0;i<=host;i++)
        {
            if(!strcmp(d_vip,vnet[i].vif))
            {
                strcpy(d_ip,vnet[i].ext);
                break;
            }
        }
        str = inet_ntoa(*(struct in_addr *)&ip_headr->saddr);
        
        if(!strcmp(str,vifip))
        {
            //encryptBlock(enpacket, &enlen, mykey);
        }
        else
            return;

        ethernet_hdr = CreateEthernetHdr(mymac, destmac, ETHERTYPE_IP);

	/* Create IP Header */

	final_ip_hdr = CreateIPHeader(enlen,d_ip);

	/* Packet length = ETH + IP header */

	final_len = sizeof(struct ethhdr) + sizeof(struct iphdr) + enlen;

	/* Allocate memory */

	final_packet = (unsigned char *)malloc(final_len);

	/* Copy the Ethernet header first */

	memcpy(final_packet, ethernet_hdr, sizeof(struct ethhdr));

	/* Copy the IP header -- but after the ethernet header */

	memcpy((final_packet + sizeof(struct ethhdr)), final_ip_hdr, sizeof(struct iphdr));

        memcpy((final_packet + sizeof(struct ethhdr))+sizeof(struct iphdr), enpacket, enlen);

	/* send the packet on the wire */

	if(!SendRawPacket(ext_sock, final_packet, final_len))
	{
		perror("Error sending packet");
	}

	/* Free the headers back to the heavenly heap */
        free(enpacket);
        free(final_packet);
        free(ethernet_hdr);
        free(final_ip_hdr);

        enpacket=NULL;
        final_packet=NULL;
        ethernet_hdr=NULL;
        final_ip_hdr=NULL;
}

unsigned char* CreateEthernetHdr(unsigned char *src_mac,unsigned char *dst_mac, int protocol)
{
	struct ethhdr *ethernet_header;


	ethernet_header = (struct ethhdr *)malloc(sizeof(struct ethhdr));
        if (ethernet_header == NULL)
        {
            puts("Memory allocation failed.\n");
            return 0;
        }

	// copy the Src mac addr

	memcpy(ethernet_header->h_source, src_mac, 6);

	// copy the Dst mac addr

	memcpy(ethernet_header->h_dest, dst_mac, 6);

	// copy the protocol

	ethernet_header->h_proto = htons(protocol);

	// done ...send the header back

	return ((unsigned char *)ethernet_header);


}

unsigned short ComputeIpChecksum(unsigned short *ptr, int length)
{
register int sum = 0;
u_short answer = 0;
register u_short *w = ptr;
register int nleft = length;

while(nleft > 1){
sum += *w++;
nleft -= 2;
}

sum = (sum >> 16) + (sum & 0xFFFF);
sum += (sum >> 16);
answer = ~sum;
return(answer);
}



unsigned char *CreateIPHeader(int enlen,unsigned char *d_ip)
{
	struct iphdr *ip_header;

	ip_header = (struct iphdr *)malloc(sizeof(struct iphdr));
        if (ip_header == NULL)
        {
            puts("Memory allocation failed.\n");
            return 0;
        }

	ip_header->version = 4;
	ip_header->ihl = (sizeof(struct iphdr))/4 ;
	ip_header->tos = 0;
	ip_header->tot_len = htons(sizeof(struct iphdr)+enlen);
	ip_header->id = htons(111);
	ip_header->frag_off = 0;
	ip_header->ttl = 100;
	ip_header->protocol = IPPROTO_RAW;
	ip_header->check = 0; /* We will calculate the checksum later */
	ip_header->saddr = inet_addr(extip);
	ip_header->daddr = inet_addr(d_ip);


	/* Calculate the IP checksum now :
	   The IP Checksum is only over the IP header */

	ip_header->check = ComputeIpChecksum((unsigned char *)ip_header, ip_header->ihl*4);

	return ((unsigned char *)ip_header);

}


int SendRawPacket(int sock, unsigned char *pkt, int pkt_len)
{
	int sent= 0;
       	/* A simple write on the socket ..thats all it takes ! */

	if((sent = write(sock, pkt, pkt_len)) != pkt_len)
	{
		return 0;
	}

	return 1;
}