#ifndef PUB_H_
#define PUB_H_

void setdaemon();
void catch_Signal(int Sign);
int signal1(int signo, void (*func)(int));
int socket_create(int port);
void socket_accept(int st);
//void sockaddr_toa(const struct sockaddr_in* addr,char* IP );
const char* getfiletype(const char* command);
#endif
