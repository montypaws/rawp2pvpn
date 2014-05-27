#include <linux/if_tun.h>
#include "globals.h"

int vif_alloc(char *dev, int flags)
{

  struct ifreq ifr;
  char command[100];
  int fd, err;
  char *clonedev = "/dev/net/tun";

  printf("Creating Virtual Interface...\n");      
  
   if( (fd = open(clonedev, O_RDWR)) < 0 )
   {
     return fd;
   }

   memset(&ifr, 0, sizeof(ifr));

   ifr.ifr_flags = flags;

   if (*dev)
   {
     strncpy(ifr.ifr_name, dev, IFNAMSIZ);
   }

   if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 )
   {
     close(fd);
     return err;
   }

  strcpy(dev, ifr.ifr_name);
  
  sprintf(command,"ifconfig %s %s netmask %s",vifname,vifip,subnet);
  system(command);
  
  printf("Virtual Interface Created.\n");
  
  return fd;
}
