/* 
 * File:   main.c
 * Author: prenx
 *
 * Created on March 13, 2011, 2:43 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "parse.c"
#include "scan_vif.c"
#include "scan_ext.c"
#include "socket.c"
#include "vir_if.c"
#include "get_MAC.c"

unsigned char destmac[6];
unsigned char mymac[6];
unsigned char extname[16];
unsigned char extip[16];
unsigned char gateway[16];
unsigned char vifname[16];
unsigned char vifip[16];
unsigned char netadd[16];
unsigned char subnet[16];
int host;
vif_net *vnet;

int main()
{
    unsigned char packet_buffer[2048]="";
    int len;
    int vif_sock,ext_sock,vfd;
    struct sockaddr_ll packet_info;
    int packet_info_size = sizeof(packet_info);
    pid_t pid;
    
    printf("Starting RAWLINK...\n");
    
    parse_conf();

    vfd=vif_alloc(vifname, IFF_TUN | IFF_NO_PI);

    printf("Binding Virtual Socket...\n");
    vif_sock = CreateRawSocket(ETH_P_ALL);
    BindRawSocketToInterface(vifname, vif_sock, ETH_P_ALL);

    printf("Binding External Socket...\n");
    ext_sock=CreateRawSocket(ETH_P_ALL);
    BindRawSocketToInterface(extname,ext_sock,ETH_P_ALL);
    
    GetMyMAC(ext_sock);
    printf("RAWLINK is Online...\n");

switch(pid = fork())
{
    case -1:
        perror("fork");
        exit(1);

    default:
    {
        while(1)
	{
		if((len = recvfrom(vif_sock, packet_buffer, 2048, 0, (struct sockaddr*)&packet_info, &packet_info_size)) == -1)
		{
			perror("Recv from returned -1: ");
			exit(-1);
		}
		else
		{
                        ParseVirtual(packet_buffer, len,ext_sock);
                }
        }
    case 0:
    {
        while(1)
	{
		if((len = recvfrom(ext_sock, packet_buffer, 2048, 0, (struct sockaddr*)&packet_info, &packet_info_size)) == -1)
		{
			perror("Recv from returned -1: ");
			exit(-1);
		}
		else
		{
                        ParseExternal(packet_buffer, len,vfd);
                }
        }
   
    close(vif_sock);
    close(ext_sock);
    return (EXIT_SUCCESS);
    }
    }
}
}
