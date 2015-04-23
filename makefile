all:
	  gcc -o client client.c -lpthread -l sqlite3

	  gcc -o server server.c -lpthread -l sqlite3
