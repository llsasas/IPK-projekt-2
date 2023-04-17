all: ipkcpd

ipkcpd:
	g++ -Wall -Wextra ./src/ipkcpd.c -o ipkcpd