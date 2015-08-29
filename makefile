.SUFFIXES:.c .o

CC=gcc

SRCS=pub.c \
     server.c \
     work.c 
OBJS=$(SRCS:.c=.o)

EXEC=myhttpd

ALL: $(OBJS)
	$(CC) -o $(EXEC) $(OBJS) -lpthread
	@echo "----------------------ok-----------------------"

.c.o:
	$(CC) -Wall -g -o $@ -c $<

clean:
	rm -f core*
	rm -f $(OBJS)
