#! /bin/bash
inputfile="./input4.txt"
inputkey="kharagpur"
while IFS= read -r line
do
    if [[ "$line" =~ "$inputkey" ]]
    then
        echo "$line" | sed 's/[a-zA-Z]/\U&/g' | sed 's/\([a-zA-Z]\)\([^a-zA-Z]*\)\([a-zA-Z]\)/\U\1\2\L\3/g'  >> output.txt
    else
        if [[ "$line" != "" ]]
        then
            echo "$line" >> output.txt
        fi
    fi
done < "$inputfile"
