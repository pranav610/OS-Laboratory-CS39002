#!/bin/bash
read -p "Folder name: " folder_name
cd $folder_name

for file in `find . -type f -name "*.py"`
do
    echo "File: $file"
    i=1
    multi_line_start=0
    while read line;
    do
        if [[ $line == \#* ]]
        then
            echo "$i: $line"
        elif [[ $line == \"\"\"* ]]
        then
            if [ $multi_line_start == 1 ]
            then
                multi_line_start=0
            else
                echo "$i: $line"
                multi_line_start=1
            fi
        fi
        i=$((i+1))
    done < $file
done
