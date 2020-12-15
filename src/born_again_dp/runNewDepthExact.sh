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
mkdir ../output/trees10/${n}/ExactDepth
mkdir ../output/trees10/${n}/Results_${n}
for u in {1..10}
do
for v in {0..2..2}
do
./bornAgain ../resources/forests/${n}/${n}.RF$u.txt ../output/trees10/${n}/ExactDepth/${n}.BA$u.O5.T10.V$v -trees 10 -obj 5 -value $v -depth ${depth[$i]}
done
i=$(($i+1));
done
done
