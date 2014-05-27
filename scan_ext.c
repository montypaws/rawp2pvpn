#include "globals.h"



ParseExternal(unsigned char *packet, int len,int vif_sock)
{
    struct ethhdr *ethernet_header;
    struct iphdr *ip_header;
    unsigned char *enpacket;
    int enlen=len-(sizeof(struct ethhdr) + sizeof(struct iphdr));
    char mykey[16]="secretkey";
    char *str;
    
    if(len >= (sizeof(struct ethhdr) + sizeof(struct iphdr)))
    {
	ethernet_header = (struct ethhdr *)packet;
		/* First set of 6 bytes are Destination MAC */
        if(ntohs(ethernet_header->h_proto) == ETH_P_IP)
        {
		ip_header = (struct iphdr*)(packet + sizeof(struct ethhdr));
                str = inet_ntoa(*(struct in_addr *)&ip_header->daddr);
                if(!strcmp(str,extip))
                {
                    if(ip_header->protocol==IPPROTO_RAW)
                    {
                        enpacket = (unsigned char*)calloc(len, sizeof(unsigned char));
                        
                        if (enpacket == NULL)
                        {
                                puts("Memory allocation failed.");
                                return 0;
                        }
                        memcpy(enpacket,packet+(sizeof(struct ethhdr) + sizeof(struct iphdr)),enlen);
                        
                        //decryptBlock(enpacket, &enlen, mykey);

                        if(!SendRawPacket(vif_sock, enpacket, enlen))
                        {
                                perror("Error sending packet");
                        }
                        
                        free(enpacket);
                        enpacket=NULL;
                    }
                }
        }
	
    }
}