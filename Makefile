CC=gcc

db: main.c
	$(CC) -o db main.c

clean:
	rm -rf db
