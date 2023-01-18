#!/bin/bash
mkdir -p ./$2
for file in ./$1/*.txt
do
    while read line;
    do
        firstLetter=${line:0:1}
        echo $line >> ./output/$firstLetter.txt
    done < $file
done 

for x in {a..z}
do
    touch ./$2/$x.txt
    sort -o ./$2/$x.txt{,}
done
