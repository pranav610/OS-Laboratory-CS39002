while IFS= read -r line
do
    if echo "$line" | grep -qw "$2";
    then
        echo "$line" | sed 's/[a-zA-Z]/\U&/g' | sed 's/\([a-zA-Z]\)\([^a-zA-Z]*\)\([a-zA-Z]\)/\U\1\2\L\3/g'
    else
            echo "$line"     
    fi
done < "$1"
