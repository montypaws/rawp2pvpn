#include "globals.h"
#define NUM_CONF 10

typedef struct conf_val
{
    unsigned char name[16];
    unsigned char value[16];
}conf_val;

parse_conf()
{
        FILE *file;
        char line[256];
        int linenum=1,i=0,j=0,flag=0,host=0;
        conf_val values[NUM_CONF];
        
        printf("Parsing Configuration file...\n");

        memset(values,0,sizeof(conf_val)*NUM_CONF);
        
        if((file=fopen("rawlink.conf","r"))== NULL)
        {
                printf("Error opening configuration file.\n");
                exit(1);
        }
        
        
        while(fgets(line, 256, file) != EOF)
        {
		printf("in");
                if(line[0] == '#') 
                {
                    linenum++;
                    continue;
                }
                if(line[0]==' ' || line[0]=='\n' || line[0]=='\t')
                {
                    linenum++;
                    continue;
                }
                if(line[0]=='>')
                {
                    flag=1;
                    continue;
                }
                
                if(line[0]=='<')
                    break;
                
                if(flag)
                    continue;
                
                if(sscanf(line, "%s %s", &values[i].name, &values[i].value)!=2)
                {
                        linenum++;
                        continue;
                }
                if(!strcmp("hosts",values[i].name))
                        host=atoi(values[i].value);
                
                if(!strcmp("extname",values[i].name))
                    strcpy(extname,values[i].value);
                
                if(!strcmp("extip",values[i].name))
                    strcpy(extip,values[i].value);
                
                if(!strcmp("gateway",values[i].name))
                    strcpy(gateway,values[i].value);
                
                if(!strcmp("vifname",values[i].name))
                    strcpy(vifname,values[i].value);
                
                if(!strcmp("vifip",values[i].name))
                    strcpy(vifip,values[i].value);
                
                if(!strcmp("netadd",values[i].name))
                    strcpy(netadd,values[i].value);
                
                if(!strcmp("subnet",values[i].name))
                    strcpy(subnet,values[i].value);
                
                linenum++;
                i++;
        }
        vnet=(vif_net *)calloc(host,sizeof(vif_net));
        rewind ( file );
        flag=0;
        i=0;
        while(fgets(line, 256, file) != EOF)
        {
            if(line[0]=='>')
                flag=1;
            else if(line[0]=='<')
                break;

            if(flag)
            {
                if(sscanf(line, "%s %s", &vnet[i].vif, &vnet[i].ext)!=2)
                        continue;
                i++;
            }
            else
                continue;
        }
        close(file);
        printf("Configuration Values Initialized.\n");
}
