#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "pub.h"


int main(int arg,char *args[])
{
	if(arg < 2)
	{
	   printf("usage:myserver port");
	   return EXIT_FAILURE;
	}
	int iport = atoi(args[1]);
	if(iport == 0)
	{
	    printf("port %d is invaild\n",iport);
	    return EXIT_FAILURE;
	}
	int st = socket_create(iport);
	if(st == 0)
	{
	   return EXIT_FAILURE;
	}
	printf("myhttp is begin");
	setdaemon();
	signal1(SIGINT,catch_Signal);
	socket_accept(st);
	close(st);
	printf("myhttpd is end\n");
	return EXIT_FAILURE;
}
