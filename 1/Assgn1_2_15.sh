#!/bin/bash
while IFS= read -r u; do
    if echo $u|grep -qP "(?=^[a-zA-Z][a-zA-Z0-9]*$)(?=.*[0-9])(?=^.{5,20}$)";then
        awk '{if(!(s+0) && index(tolower(var),tolower($1))){print"NO";s=1;}};END{if(!(s+0))print"YES";}' var="$u" fruits.txt
    else 
        echo "NO"
    fi 
done <usernames.txt>"$1"
