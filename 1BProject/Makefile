#NAME: Matei Lupu
#ID: 504798407
#EMAIL: mateilupu20@g.ucla.edu

default: client server

client:
	gcc -Wall -Wextra -g -o lab1b-client lab1b-client.c
server:
	gcc -Wall -Wextra -g -o lab1b-server lab1b-server.c
clean:
	rm -rf lab1-client lab1-server *.tar.gz
dist:
	tar -czvf lab1b-504798407.tar.gz lab1b-client.c lab1b-server.c Makefile README my.key

#	gcc -o cliserv lab1b-client lab1b-server
