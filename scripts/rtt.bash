#!/bin/bash

set -e

declare size
echo -n "Seleziona la size dei messaggi: 16/32/1024/2048: "
read -e size

#Selezioniamo la size con cui fare l'esperimento
if [ "$size" == "16" ] || [ "$size" == "32" ] || [ "$size" == "1024" ] || [ "$size" == "2048" ];  then
	echo "Hai selezionato $size bytes"
else
	echo "Size selezionata invalida"
	exit 1
fi

declare input="../data/udp_${size}.out"   #a seconda della size prendiamo un file di input diverso
declare values="../data/rtt_udp_timeout_${size}.out  "#file di output
declare search_string='for'
declare repetitions
declare line
declare counter
declare rtt
declare estimatedRTT
declare variabilityRTT
declare abs_value

repetitions=150

> "${values}" #svuoto il file
variabilityRTT=0 #settiamo la variability a zero per il primo valore
counter=1

#eseguo un loop per calcolare valori di estimatedRTT e variabilityRTT
until [ $counter -gt $repetitions ]
do
line=$(grep -m $counter 'milliseconds' $input | tail -n1)
rtt=$(echo $line| head -n1 | cut -d " " -f5)
if [ "$counter" == "1" ]; then
  estimatedRTT=$rtt #settiamo il primo valore dell'estimated uguale al primo valore di RTT reale
else
  estimatedRTT=$(echo "scale=10;(0.875*$estimatedRTT)+(0.125*$rtt)" | bc) #calcolo l'estimatedRTT con bc
fi

abs_value=$(echo "scale=10;($rtt - $estimatedRTT)" | bc)
abs_value=$(echo ${abs_value#-})
variabilityRTT=$(echo "(0.75*$variabilityRTT) + (0.25*$abs_value)" | bc) #calcoliamo la variabilityRTT con bc
echo ${counter} ${rtt} ${estimatedRTT} ${variabilityRTT} >> "${values}" #inseriamo dentro il file di output i valori 
((counter++))
done
#fine loop
gnuplot <<-eNDgNUPLOTcOMMAND
	set term png size 900, 700
	set output "../data/RTT_${size}.png"
  set xrange[1:$repetitions]
  set logscale y 10
	set xlabel "Repetitions"
	set ylabel "RTT (milliseconds)"
	plot "$values" using 1:2 title "RTT" \
			with linespoints, \
		"$values" using 1:3 title "estimatedRTT" \
			with linespoints, \
		"$values" using 1:4 title "variabilityRTT" \
			with linespoints
	clear
eNDgNUPLOTcOMMAND
