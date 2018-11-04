all: server client

server : 2013726003_s.c
	gcc 2013726003_s.c -o server -lncurses
client : 2013726003_c.c
	gcc 2013726003_c.c -o client -lncurses
