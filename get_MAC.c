#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <netinet/ether.h>
#include <net/ethernet.h>
#include "globals.h"


#define SRC_ETHER_ADDR	"aa:aa:aa:aa:aa:aa"
#define BROADCAST_ADDR "ff:ff:ff:ff:ff:ff"
#define DST_ETHER_ADDR	"00:00:00:00:00:00"
#define SRC_IP	"192.168.1.2"
#define DST_IP	"192.168.1.1"

typedef struct EthernetHeader
{
	unsigned char destination[6];
	unsigned char source[6];
	unsigned short protocol;
}EthernetHeader;

typedef struct ArpHeader
{
	unsigned short hardware_type;
	unsigned short protocol_type;
	unsigned char hard_addr_len;
	unsigned char prot_addr_len;
	unsigned short opcode;
	unsigned char source_hardware[6];
	unsigned char source_ip[4];
	unsigned char dest_hardware[6];
	unsigned char dest_ip[4];
}ArpHeader;

EthernetHeader* CreateEthernetHeader(char *src_mac, char *dst_mac, int protocol)
{
	EthernetHeader  *ethernet_headr=NULL;
	ethernet_headr = (EthernetHeader *)malloc(sizeof(EthernetHeader));
        if (ethernet_headr == NULL)
        {
            puts("Memory allocation failed.\n");
            return 0;
        }
	/* copy the Src mac addr */
        memcpy(ethernet_headr->destination, (void *)ether_aton(dst_mac), 6);

	memcpy(ethernet_headr->source, src_mac, 6);

	/* copy the Dst mac addr */

	

	/* copy the protocol */

	ethernet_headr->protocol = htons(protocol);

	/* done ...send the header back */

	return ethernet_headr;
}

ArpHeader *CreateArpHeader(char *mymac)
{
	ArpHeader *arp_headr=NULL;
	in_addr_t temp;

	arp_headr = (ArpHeader *)malloc(sizeof(struct ArpHeader));
        if (arp_headr == NULL)
        {
            puts("Memory allocation failed.\n");
            return 0;
        }

	/* Fill the ARP header */
	arp_headr->hardware_type = htons(ARPHRD_ETHER);
	arp_headr->protocol_type = htons(ETHERTYPE_IP);
	arp_headr->hard_addr_len = 6;
	arp_headr->prot_addr_len = 4;
	arp_headr->opcode = htons(ARPOP_REQUEST);
	memcpy(arp_headr->source_hardware,mymac , 6);
	temp = inet_addr(extip);
	memcpy(&(arp_headr->source_ip), &temp, 4);
	memcpy(arp_headr->dest_hardware, (void *) ether_aton(DST_ETHER_ADDR) , 6);
	temp = inet_addr(gateway);
	memcpy(&(arp_headr->dest_ip), &temp, 4);

	return arp_headr;
}
int GetMyMAC(int sock)
{
	struct ifreq buffer;
        unsigned char *packet=NULL;
	EthernetHeader *ethernet_header=NULL;
	ArpHeader *arp_header=NULL;
	int pkt_len,s,len,i=0;
        struct sockaddr_ll packet_info;
        int packet_info_size = sizeof(packet_info);
        
        unsigned char *rpacket=NULL;
        EthernetHeader *rethernet=NULL;
        ArpHeader *rarp=NULL;

        printf("Getting MAC addresses...\n");
	memset(&buffer, 0x00, sizeof(buffer));

	strcpy(buffer.ifr_name, extname);

	ioctl(sock, SIOCGIFHWADDR, &buffer);
        printf("System MAC address-");
	for( s = 0; s < 6; s++ )
	{
		mymac[s]= buffer.ifr_ifru.ifru_hwaddr.sa_data[s];
                printf("%02x:", mymac[s]);
        }
        printf("\n");

        ethernet_header = CreateEthernetHeader(mymac, BROADCAST_ADDR , ETHERTYPE_ARP);

	/* Create ARP header */

	arp_header = CreateArpHeader(mymac);

	/* Find packet length  */

	pkt_len = sizeof(EthernetHeader) + sizeof(ArpHeader);

	/* Allocate memory to packet */
        rpacket = (unsigned char *)malloc(pkt_len);
        if (rpacket == NULL)
        {
            puts("Memory allocation failed.\n");
            return 0;
        }
	packet = (unsigned char *)malloc(pkt_len);
        if (packet == NULL)
        {
            puts("Memory allocation failed.\n");
            return 0;
        }

	/* Copy the Ethernet header first */

	memcpy(packet, ethernet_header, sizeof(EthernetHeader));

	/* Copy the ARP header - but after the ethernet header */

	memcpy((packet + sizeof(EthernetHeader)), arp_header, sizeof(ArpHeader));

	/* Send the packet out ! */

	if(!SendRawPacket(sock, packet, pkt_len))
	{
		perror("Error sending packet");
	}
        
	/* Free the memory back to the heavenly heap */
        free(packet);	
        free(ethernet_header);
	free(arp_header);
        
        packet=NULL;
        ethernet_header=NULL;
        arp_header=NULL;

        while(1)
        {
            if((len = recvfrom(sock, rpacket, 60, 0, (struct sockaddr*)&packet_info, &packet_info_size)) == -1)
            {
                    perror("Recv from returned -1: ");
                    exit(-1);
            }
            else
            {
                if(len > sizeof(struct EthernetHeader))
                {
                    rethernet = (struct EthernetHeader *)rpacket;
                    if(ntohs(rethernet->protocol) != ETH_P_ARP)
                    {
                        continue;
                    }
                    else
                    {
                        if(len >= (sizeof(struct EthernetHeader) + sizeof(struct ArpHeader)))
                        {
                            rarp = (struct ArpHeader*)(rpacket + sizeof(struct EthernetHeader));
                            memcpy(destmac, rarp->source_hardware,6);
                            printf("Default Gateway MAC address-");
                            for(s=0;s<6;s++)
                            {
                                printf("%02x:",destmac[s]);
                            }
                            printf("\n");
                            break;                            
                        }
                    }
                }
            }
        }
        return 0;
}