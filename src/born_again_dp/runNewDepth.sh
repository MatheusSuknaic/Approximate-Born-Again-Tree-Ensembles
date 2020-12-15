#!/bin/bash
i=0
depth=()
input="depths.txt"
while IFS= read -r line
do
 	#echo "$line"
	depth+=($line)
done < "$input"

for n in COMPAS-ProPublica FICO HTRU2
do
for u in {1..10}
do
for v in {0..2}
do
mkdir ../output/trees10/${n}/${n}.BA$u.O5.T10.V$v
currdepth=${depth[$i]}
while [ $currdepth -ge 0 ]
do
./bornAgain ../resources/forests/$n/${n}.RF$u.txt ../output/trees10/${n}/${n}.BA$u.O5.T10.V$v/${n}.BA$u.O5.T10.V$v.D$currdepth -trees 10 -obj 5 -value $v -depth $currdepth
currdepth=$(($currdepth-1))
done
done
i=$(($i+1));
done
done
