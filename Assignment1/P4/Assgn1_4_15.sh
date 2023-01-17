#! /bin/bash

inputfile="./input4.txt"
inputkey="kharagpur"
while read line
do
    if [[ $line =~ $inputkey ]]
    then
        echo $line | sed 's/[a-zA-Z]/\U&/g' | sed 's/\([a-zA-Z]\)[^a-zA-Z]*\([a-zA-Z]\)/\U\1\L\2/g'  >> output.txt
    else
        if [[ $line != '' ]]
        then
            echo $line >> output.txt
        fi
    fi
done < $inputfile
