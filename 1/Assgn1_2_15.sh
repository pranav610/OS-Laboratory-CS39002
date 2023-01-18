#!/bin/bash
while IFS= read -r u; do
    if echo $u|grep -qP "^(?=[a-zA-Z])(?=.*[0-9])(?=.{4,19})";then
        awk '{if(s+0==0 && index(tolower(var),tolower($1))){print"NO";s=1;}};END{if(s+0==0)print"YES";}' var="$u" fruits.txt
    else 
        echo "NO"
    fi 
done <usernames.txt>"$1"
