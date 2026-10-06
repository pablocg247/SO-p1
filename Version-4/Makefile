CC = gcc
CFLAGS = -g -Wall

shell: Shellv4.c ejemplo.o path.o listasimple.o openfileslist.o
	$(CC) -o shell $(CFLAGS) Shellv4.c  ejemplo.o path.o listasimple.o openfileslist.o

ejemplo.o: ejemplo.c ejemplo.h path.h openfileslist.h
	$(CC) -c $(CFLAGS) ejemplo.c

openfileslist.o: openfileslist.c openfileslist.h listasimple.h
	$(CC) -c $(CFLAGS) openfileslist.c

path.o: path.c path.h listasimple.h
	$(CC) -c $(CFLAGS) path.c

listasimple.o: listasimple.c listasimple.h
	$(CC) -c $(CFLAGS) listasimple.c

clean:
	rm shell *.o
