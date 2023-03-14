#!/bin/bash
mkdir -p ./$2
for file in ./$1/*.txt
do
    while read line;
    do
        echo $line >> ./output/${line:0:1}.txt
    done < $file
done 

for x in {a..z}
do
    touch ./$2/$x.txt
    sort -o ./$2/$x.txt{,}
don
