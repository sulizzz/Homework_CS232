CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -lm

OBJS = webSearch.o crawler.o index.o

webSearch: $(OBJS)
	$(CC) $(CFLAGS) -o webSearch $(OBJS) $(LDFLAGS)

webSearch.o: webSearch.c crawler.h index.h
	$(CC) $(CFLAGS) -c webSearch.c

crawler.o: crawler.c crawler.h
	$(CC) $(CFLAGS) -c crawler.c

index.o: index.c index.h
	$(CC) $(CFLAGS) -c index.c

clean:
	rm -f *.o webSearch
