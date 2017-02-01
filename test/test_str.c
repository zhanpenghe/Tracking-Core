#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void str_split(char *buf)
{
	char *beacon_mac, *rssi_str, *temp;

	temp = strtok_r(buf, "|", &buf);
	beacon_mac = temp;
	temp = strtok_r(NULL, "|", &buf);
	rssi_str = temp;
	temp = strtok_r(NULL,"|", &buf);
		
	printf("%s..%s..%s..\n" ,beacon_mac, rssi_str, temp);
}

void main()
{
	char msg[] = "B8:27:EB:DB:0C:FB|2\nFE:D9:0F:07:C3:3D|-73|1485971715.1036\nFE:D9:0F:07:C3:3D|-71|1485971715.2115";
	char agent_mac[18];
	char buf[39];
	char *line, *temp;

	strncpy(agent_mac, msg, 17);
	agent_mac[17] = 0;

	line = strtok(msg,"\n");
	printf("%s\n", line);

	while((line = strtok(NULL, "\n"))!=NULL)
	{
		memcpy(buf,line, 39);
		printf("--%s.. %d\n", buf, (int)strlen(buf));
		
		str_split(buf);
	}

}

