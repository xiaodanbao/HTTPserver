#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <fcntl.h>
#include "work.h"
#include "pub.h"

int socket_create(int port)
{
	int st = socket(AF_INET, SOCK_STREAM, 0);
	if (st == -1)
	{
		printf("socket failed %s\n", strerror(errno));
		return 0;
	}
	int on = 1;
	if (setsockopt(st, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
	{
		printf("setsockopt failed %s\n", strerror(errno));
		return 0;
	}
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(st, (struct sockaddr *) &addr, sizeof(addr)) == -1)
	{
		printf("bind failed %s\n", strerror(errno));
		return 0;
	}
	if (listen(st, 100) == -1)
	{
		printf("listen failed %s\n", strerror(errno));
		return 0;
	}
	printf("listen %d success\n", port);
	return st;
}

void setdaemon()
{
	pid_t pid, sid;
	pid = fork();
	if (pid < 0)
	{
		printf("fork failed %s\n", strerror(errno));
		exit (EXIT_FAILURE);
		;
	}
	if (pid > 0)
	{
		exit (EXIT_SUCCESS);
	}

	if ((sid = setsid()) < 0)
	{
		printf("setsid failed %s\n", strerror(errno));
		exit (EXIT_FAILURE);
	}

	/*
 * 	 if (chdir("/") < 0)
 * 	 	 {
 * 	 	 	 printf("chdir failed %s\n", strerror(errno));
 * 	 	 	 	 exit(EXIT_FAILURE);
 * 	 	 	 	 	 }
 * 	 	 	 	 	 	 umask(0);
 * 	 	 	 	 	 	 	 close(STDIN_FILENO);
 * 	 	 	 	 	 	 	 	 close(STDOUT_FILENO);
 * 	 	 	 	 	 	 	 	 	 close(STDERR_FILENO);
 * 	 	 	 	 	 	 	 	 	 	 */
}

void catch_Signal(int Sign)
{
	switch (Sign)
	{
	case SIGINT:
		printf("signal SIGINT\n");
		break;
	}
}
const char* getfiletype(const char* command)
{
	int i=strlen(command)-1;
	char sExt[32];
	memset(sExt,0,sizeof(sExt));
	for(;i>=0;i--)
	{
	  if(command[i] =='.')
	  {
		strncpy(sExt,&command[i+1],sizeof(sExt));break;
	}
	}
	
		if (strncmp(sExt, "bmp", 3) == 0)
		return "image/bmp";

	if (strncmp(sExt, "gif", 3) == 0)
		return "image/gif";

	if (strncmp(sExt, "ico", 3) == 0)
		return "image/x-icon";

	if (strncmp(sExt, "jpg", 3) == 0)
		return "image/jpeg";

	if (strncmp(sExt, "avi", 3) == 0)
		return "video/avi";

	if (strncmp(sExt, "css", 3) == 0)
		return "text/css";

	if (strncmp(sExt, "dll", 3) == 0)
		return "application/x-msdownload";

	if (strncmp(sExt, "exe", 3) == 0)
		return "application/x-msdownload";

	if (strncmp(sExt, "dtd", 3) == 0)
		return "text/xml";

	if (strncmp(sExt, "mp3", 3) == 0)
		return "audio/mp3";

	if (strncmp(sExt, "mpg", 3) == 0)
		return "video/mpg";

	if (strncmp(sExt, "png", 3) == 0)
		return "image/png";

	if (strncmp(sExt, "ppt", 3) == 0)
		return "application/vnd.ms-powerpoint";

	if (strncmp(sExt, "xls", 3) == 0)
		return "application/vnd.ms-excel";

	if (strncmp(sExt, "doc", 3) == 0)
		return "application/msword";

	if (strncmp(sExt, "mp4", 3) == 0)
		return "video/mpeg4";

	if (strncmp(sExt, "ppt", 3) == 0)
		return "application/x-ppt";

	if (strncmp(sExt, "wma", 3) == 0)
		return "audio/x-ms-wma";

	if (strncmp(sExt, "wmv", 3) == 0)
		return "video/x-ms-wmv";

	return "text/html";
	
}

void sockaddr_toa(const struct sockaddr_in* addr,char* IP )
{
	unsigned char *p = (unsigned char*)&(addr->sin_addr.s_addr);
	sprintf(IP,"%u.%u.%u.%u",p[0],p[1],p[2],p[3]);
}
int signal1(int signo, void (*func)(int))
{
	struct sigaction act, oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	return sigaction(signo, &act, &oact);
}
void  socket_accept(int st)
{
	pthread_t thr_d;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	
	int client_st = 0;
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	memset(&client_addr,0,sizeof(client_addr));
	while(1)
	{
	    memset(&client_addr,0,sizeof(client_addr));
	    client_st = accept(st,(struct sockaddr*)&client_addr,&len);
	    static int count = 0;
	    if(count >= 200)
	    { 
		close(client_st);
		continue;
	    }
	    else count++;
	    if(client_st == -1)
	    {
		printf("accept failed: %s\n",strerror(errno));
		break;
	    }
	    else 
	    {
		 char sIP[32];
		  memset(&sIP,0,sizeof(sIP));
		  sockaddr_toa(&client_addr,sIP);
		  printf("accept by %s",sIP);
		  
		  int *tmp = (int*)malloc(sizeof(int));
		  *tmp = client_st;
		  
		  pthread_create(&thr_d,&attr,socket_contr,tmp);
		  
		}
	     
	} 
	pthread_attr_destroy(&attr);
}




