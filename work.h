#ifndef WORK_H_
#define WORK_H_

void gethttpcommand(const char* httpMsg,char *command);

void* socket_contr(void *arg);

int getfilecontent(const char* filename,char** content);

#endif
