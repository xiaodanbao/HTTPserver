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
#include "work.h"
#include "pub.h"


#define BUFSIZE 8192

#define HEAD "HTTP/1.0 200 OK\n\
Content-Type: %s\n\
Transfer-Encoding: chunked\n\
Connection: Keep-Alive\n\
Accept-Ranges:bytes\n\
Content-Length:%d\n\n"


#define TAIL "\n\n"

#define EXEC "s?wd"

void gethttpcommand(const char* httpMsg,char *command)
{
	int end = 0;
	int start = 0;
	int i=0;
	for(i=0;i<strlen(httpMsg);i++)
	{
	    if( (httpMsg[i] == ' ') && (start == 0))
	    {
		start = i+2;
		}
	    else 
	    {
		if(httpMsg[i] == ' ')
		  {end = i;break;}
		}
	}
 	   strncpy(command,&httpMsg[start],(end-start));
}
int getfilecontent(const char* filename,char** content)
{
	FILE* fp = fopen(filename,"rb");
	struct stat t;
	memset(&t,0,sizeof(t));
	if(fp == NULL)
	{
	   printf("fopen %s failed %s\n",filename,strerror(errno));
	   return 0;
	}
	else 
	{
	   stat(filename,&t);
	   *content = malloc(t.st_size);
	   fread(*content,t.st_size,1,fp);
	   fclose(fp) ;
	   return t.st_size; 
	   
	}
}
int make_http_content(const char* command,char**buf)
{
	char *content = NULL;
	int icontentlen = 0;
	if(command[0] == 0)
	{
	    icontentlen = getfilecontent("default.html",&content);   
	}
	else 
	{
	    icontentlen = getfilecontent(command,&content);
	}
	if(icontentlen > 0)
	{
	   char headbuf[1024];
	   memset(&headbuf,0,sizeof(headbuf));
	   sprintf(headbuf,HEAD,getfiletype(command),icontentlen);
	   int headlen = strlen(headbuf);
	   int taillen = strlen(TAIL);
	   int sumlen = headlen + taillen + icontentlen;
	   *buf = malloc(sumlen);
	   char *tmp = *buf;
	   memcpy(tmp,headbuf,headlen);
	   memcpy(&tmp[headlen],content,icontentlen);
	   memcpy(&tmp[headlen+icontentlen],TAIL,taillen);
	   printf("headbuf:\n%s",headbuf);
	  
	   if(content)
		free(content);
	   return sumlen;
	}
	else 
	{
	   return 0;
	}
}


void* socket_contr(void *arg)
{
	printf("thread is begin\n");
	int st = *(int*)arg;
	free((int*)arg);
	
	char buf[BUFSIZE];
	memset(buf,0,sizeof(buf));
	int rc = recv(st,buf,sizeof(buf),0);
	if(rc <= 0)
	{
	   printf("recv failed %s\n",strerror(errno));
	}
	else 
	{
	   printf("recv:%s\n",buf);
	   char command[1024];
	   memset(command,0,sizeof(command));
	   gethttpcommand(buf,command);
	   char *content = NULL;
	   int ilen = make_http_content(command,&content);
	   if(ilen > 0)
	   {
		send(st,content,ilen,0);
		free(content);
	   }
	   
	}
	close(st);
	printf("thread is end\n");
	return NULL;
}


