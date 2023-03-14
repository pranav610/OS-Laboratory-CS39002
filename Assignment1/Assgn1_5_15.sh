#!/bin/bash
read -p "Folder name: " folder_name
cd $folder_name
temp_file="temp.txt"
touch $temp_file

for file in `find . -type f -name "*.py"`
do
    var=${file#*./}
    echo "FILE: " $var, "PATH:" `realpath $file`
    
    i=1
    multi_line_count=0
    multi_line_start=0
    while read line;
    do
        if [[ $multi_line_start == 0 ]];
        then
            echo $line > $temp_file
            double_quote=0
            single_quote=0
            single_line_start=0
            while read -n1 char; do
                if [[ $char == \# ]];
                then
                    if [ $double_quote == 0 ] && [ $single_quote == 0 ];
                    then
                        echo "$i: $line"
                        single_line_start=1
                        break
                    fi
                elif [[ $char == \" ]];
                then
                    if [ $double_quote == 0 ];
                    then
                        double_quote=1
                    else
                        double_quote=0
                    fi
                elif [[ $char == \' ]];
                then
                    if [ $single_quote == 0 ];
                    then
                        single_quote=1
                    else
                        single_quote=0
                    fi
                fi
            done < $temp_file
            if [ $single_line_start == 1 ];
            then
                i=$((i+1))
                continue
            fi
        fi

        if [ $multi_line_start == 1 ]
        then
            echo "$i: $line"
        fi

        echo $line > $temp_file
        while read -n1 char; do
            if [[ $char == \" ]];
            then
                multi_line_count=$((multi_line_count+1))
            else
                multi_line_count=0
            fi

            if [ $multi_line_count == 3 ];
            then
                if [ $multi_line_start == 1 ];
                then
                    multi_line_start=0
                    multi_line_count=0
                else
                    multi_line_start=1
                    multi_line_count=0
                    echo "$i: $line"
                fi
            fi
        done < $temp_file

        while read -n1 char; do
            if [[ $char == \' ]];
            then
                multi_line_count=$((multi_line_count+1))
            else
                multi_line_count=0
            fi

            if [ $multi_line_count == 3 ];
            then
                if [ $multi_line_start == 1 ];
                then
                    multi_line_start=0
                    multi_line_count=0
                else
                    multi_line_start=1
                    multi_line_count=0
                    echo "$i: $line"
                fi
            fi
        done < $temp_file

        i=$((i+1))
    done < $file
rm $temp_file
done