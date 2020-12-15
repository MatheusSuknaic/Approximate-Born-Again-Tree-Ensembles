#!/bin/bash
touch depths.txt
for n in COMPAS-ProPublica FICO HTRU2
do
for u in {1..10}
do
awk -F "\"*,\"*" '{print $7}' ../output/trees10/${n}/${n}.BA$u.O0.T10.out >> depths.txt
done
done
