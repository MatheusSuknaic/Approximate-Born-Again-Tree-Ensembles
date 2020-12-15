#!/bin/bash
mkdir ../output/trees10
for n in COMPAS-ProPublica FICO HTRU2
do
mkdir ../output/trees10/${n}
done
for n in COMPAS-ProPublica FICO HTRU2
do
for u in {1..10}
do
./bornAgain ../resources/forests/$n/${n}.RF$u.txt ../output/trees10/${n}/${n}.BA$u.O0.T10 -trees 10 -obj 0
done
done

