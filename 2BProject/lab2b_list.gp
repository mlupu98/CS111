#! /usr/local/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2b_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#
# output:
# lab2b_1.png: a graph of throughput vd the number of threads for both the mutex and spin-lock sync methods
# lab2b_2.png: a graph of Average time per operation and the wait-for-lock time vs the number of threads
# lab2b_3.png: a graph of Checking How Many Iterations are Needed For Failure vs the Number of Threads
# lab2b_4.png: a graph of Throughput vs the Number of Threads for the Mutex Lock
# lab2b_5.png: a graph of Throughput vs the Number of Threads for the Spin Lock
#
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep to select only the data they want.
#
#	Early in your implementation, you will not have data for all of the
#	tests, and the later sections may generate errors for missing data.
#

#general plot parameters
set terminal png
set datafile separator ","

set title "Number of Operations per Second vs. Number of Threads For Each Synchronization Method"
set xlabel "Number of Threads"
set logscale x 2
set xrange [1:25]
set ylabel "Throughput, Operations / Sec "
set logscale y 10
set output 'lab2b_1.png'

# lab2b_1.png
plot \
     "< grep 'list-none-m,[0-9]*,1000,1' lab2_list.csv" using ($2):(1000000000/$7) \
	title 'Mutex' with linespoints lc rgb 'blue', \
     "< grep 'list-none-s,[0-9]*,1000,1' lab2_list.csv" using ($2):(1000000000/$7) \
	title 'Spin Lock' with linespoints lc rgb 'green'

# lab2b_2.png, Average time per operation and the wait-for-lock time vs the number of threads
set title "Time per Operation and Wait-For-Lock Time for Mutex Lock vs the Number of Threads"
set xlabel "Number of Threads"
set logscale x 2
set xrange [1:25]
set ylabel "Time (s)"
set logscale y 10
set output 'lab2b_2.png'

plot \
    "< grep 'list-none-m,' lab2b_list.csv" using ($2):($7) \
    title 'Run Time per Operation' with points lc rgb 'blue', \
     "< grep 'list-none-m,' lab2b_list.csv" using ($2):($8) \
    title 'Wait for Lock Time' with points lc rgb 'green'
     

# lab2b_3.png
set title "Checking How Many Iterations are Needed For Failure vs the Number of Threads"
set xlabel "Number of Threads"
set logscale x 2
set xrange [1:16]
set ylabel "Operations Before Failure"
set logscale y 10
set output 'lab2b_3.png'
    # "< grep 'list-id-none,' lab2b_list.csv" using ($2):($3) with points lc rgb "red" \ 
    # title "Unprotected", \
plot \
    "< grep 'list-id-m,' lab2b_list.csv" using ($2):($3) \
    title 'Mutex' with points lc rgb 'blue', \
    "< grep 'list-id-s,' lab2b_list.csv" using ($2):($3) \
    title 'Spin' with points lc rgb 'green'


# lab2b_4.png
set title "Throughput vs the Number of Threads for the Mutex Lock"
set xlabel "Number of Threads"
unset xrange
set xrange [0.75:16]
set logscale x 2
set ylabel "Throughput"
unset yrange
set logscale y
set output 'lab2b_4.png'
plot \
     "< grep 'list-none-m,[0-9][2]\\?,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'l=1' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,[0-9][2]\\?,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'l=4' with linespoints lc rgb 'green', \
     "< grep 'list-none-m,[0-9][2]\\?,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'l=8' with linespoints lc rgb 'orange', \
     "< grep 'list-none-m,[0-9][2]\\?,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'l=16' with linespoints lc rgb 'blue'

#lab2b_5.png
set title "Throughput vs the Number of Threads for the Spin Lock"
set xlabel "Number of Threads"
unset xrange
set xrange [0.75:16]
set logscale x 2
set ylabel "Throughput"
unset yrange
set logscale y
set output 'lab2b_5.png'
plot \
     "< grep 'list-none-s,[0-9][2]\\?,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'l=1' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9][2]\\?,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'l=4' with linespoints lc rgb 'green', \
     "< grep 'list-none-s,[0-9][2]\\?,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'l=8' with linespoints lc rgb 'orange', \
     "< grep 'list-none-s,[0-9][2]\\?,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'l=16' with linespoints lc rgb 'blue'