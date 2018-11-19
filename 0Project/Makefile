#Name: Matei Lupu
#ID: 504798407
#Email: mateilupu20@g.ucla.edu
.SILENT:
default:
	gcc -Wall -Wextra -g -o lab0 lab0.c
clean:
	rm -f lab0 *.txt lab0-504798407.tar.gz *.o
dist:
	tar -czvf lab0-504798407.tar.gz lab0.c backtrace.png breakpoint.png README Makefile

check:	default fileToFile forceSegfault ensureCatch missingInFile noAccesOutFile 

fileToFile:
	echo lab0.c > out.txt; \
	echo ./lab0 --input=lab0.c --output=out.txt; \
	echo cmp lab0.c out.txt; \
	if [[ $$? -ne 0 ]]; \
	then \
		echo "file to file not passed"; \
	fi;

forceSegfault:
	echo "input" | ./lab0 --segfault; \
	if [[ $$? -ne 139 ]]; \
	then \
		echo "forceSegfault not passed"; \
	fi;

ensureCatch:
	echo "input" | ./lab0 --segfault --catch; \
	if [[ $$? -ne 4 ]]; \
	then \
		echo "ensureCatch not passed"; \
	fi;

missingInFile:
	echo rm missing.txt; \
	echo | ./lab0 --input=missing.txt; \
	if [[ $$? -ne 2 ]]; \
	then \
		echo "missingFile not passed"; \
	fi;

noAccesOutFile:
	echo lab0.c > newout.txt; \
	chmod 555 newout.txt; \
	echo "input" | ./lab0 --output=newout.txt; \
	if [[ $$? -ne 3 ]]; \
	then \
		echo "noAccesOutFile not passed"; \
	fi;
	
 
