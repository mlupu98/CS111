# NAME: Matei Lupu
# EMAIL: mateilupu20@g.ucla.edu
# ID: 504798407

.SILENT:

default: build

build: lab2_list.c SortedList.h SortedList.c
	gcc -g -Wextra -Wall -lrt -pthread -o lab2_list lab2_list.c SortedList.c

tests: default
	touch lab2_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=m --lists=4  >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=m --lists=4  >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=m --lists=4  >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=m --lists=4  >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m --lists=4  >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m --lists=4  >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m --lists=4  >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=10   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=20   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=40  --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=80   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=10   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=20   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=40   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=80   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=10   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=20   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=40   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=80   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=10   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=20   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=40   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=80   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=10   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=20   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=40   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=80   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=10   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=20   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=40   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=80   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=10   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=20   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=40   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=80   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=10   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=20   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=40   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=80   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=10   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=20   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=40   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=80   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=16  --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=10   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=20   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=40   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=80   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=1  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=1  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=1  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=1  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=1  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=8  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=8  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=8  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=8  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=8  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=16 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=16 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=16 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=16 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=16 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=1  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=1  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=1  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=1  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=1  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=8  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=8  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=8  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=8  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=8  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=16 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=16 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=16 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=16 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=16 --sync=s   >> lab2b_list.csv


profile:
	-rm -f profile.gperf 
	LD_PRELOAD=/usr/lib64/libprofiler.so CPUPROFILE=profile.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text lab2_list profile.gperf > profile.out
	pprof --list=iterateLocks lab2_list profile.gperf >> profile.out
	-rm -f profile.gperf

graphs: tests
	gnuplot lab2_list.gp
clean:
	rm -rf *.tar.gz lab2_list

dist: SortedList.h SortedList.c lab2_list.c lab2_list.gp graphs Makefile	
	tar -czvf lab2b-504798407.tar.gz SortedList.h *.png *.csv *.c lab2_list.gp profile.out Makefile README
