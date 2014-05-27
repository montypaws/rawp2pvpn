/* 
 * File:   globals.h
 * Author: root
 *
 * Created on April 19, 2011, 9:12 AM
 */

#ifndef GLOBALS_H
#define	GLOBALS_H

typedef struct vnet_entries
{
	unsigned char vif[16];
	unsigned char ext[16];
}vif_net;

extern unsigned char destmac[6],mymac[6];
extern unsigned char extname[16];
extern unsigned char extip[16];
extern unsigned char gateway[16];
extern unsigned char vifname[16];
extern unsigned char vifip[16];
extern unsigned char netadd[16];
extern unsigned char subnet[16];
extern int host;
extern vif_net *vnet;

#endif	/* GLOBALS_H */

