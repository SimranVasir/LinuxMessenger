linux:
	gcc a5thread.c -o stalk -lpthread

clean: 
	rm -f stalk stalk.o

