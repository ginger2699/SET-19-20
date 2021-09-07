#!/bin/bash

set -e

declare Protocol
declare line
declare tail_line
echo -n "Select the Protocol (tcp, udp): "
read -e Protocol
echo "You have selected a $Protocol socket"

#Si seleziona il protocollo
if [ "$Protocol" == "tcp" ]; then
	line=$(head -n 1 "../data/tcp_throughput.dat")
	tail_line=$(tail -n 1 "../data/tcp_throughput.dat")
elif [ "$Protocol" == "udp" ]; then
	line=$(head -n 1 "../data/udp_throughput.dat")
	tail_line=$(tail -n 1 "../data/udp_throughput.dat")
else
	echo "Protocollo selezionato invalido"
	exit 1
fi

declare n1
declare t1
declare n2
declare t2
declare d1
declare d2
declare B
declare L

n1=$(echo $line | cut -d' ' -f 1)
t1=$(echo $line | cut -d' ' -f 2)
n2=$(echo $tail_line | cut -d' ' -f 1)
t2=$(echo $tail_line | cut -d' ' -f 2)

if echo $t2 | grep -q "e"; then #Caso in cui il numero preso è espresso con una potenza di dieci
declare t_aux
t_aux=$(echo $t2 | cut -d'+' -f 2)
t2=$(echo $t2 | cut -d'e' -f 1)
t2=$(echo "scale=10;$t2*(10^$t_aux)" | bc )#Nel caso sia in potenza di dieci calcoliamo con bc quanto vale con una precisione a dieci cifre decimali
fi

d1=$(echo "scale=10;$n1/$t1" | bc)
d2=$(echo "scale=10;$n2/$t2" | bc)
B=$(echo "scale=10;($n2-$n1)/($d2-$d1)" | bc)
L=$(echo "scale=10;(($d1*$n2)-($d2*$n1))/($n2-$n1)" | bc)

gnuplot <<-eNDgNUPLOTcOMMAND
	set term png size 900, 700
	set output "../data/throughput_${Protocol}.png"
	set xlabel "msg size (B)"
	set ylabel "throughput (KB/s)"
	set logscale x 2   
	set logscale y 10
	set xrange [$n1:$n2]
	lbf(x) = x / ( $L + x / $B )
	plot lbf(x) title "Latency-Bandwidth model with L=$L and B=$B" with linespoints, \
		"../data/${Protocol}_throughput.dat" using 1:2 title "${Protocol} median Throughput" \
			with linespoints
	clear
eNDgNUPLOTcOMMAND
