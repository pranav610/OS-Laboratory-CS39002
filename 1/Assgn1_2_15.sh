#!/bin/bash
while IFS= read -r u; do
    echo "$u" | grep -qP "^(?=[a-zA-Z])(?=.*[0-9])(?=.{4,19})" || echo "NO" && awk '{if(s+0==0 && index(tolower(var),tolower($1))){print"NO";s=1;}};END{if(s+0==0)print"YES";}' var="$u" fruits.txt 
done <usernames.txt>"$1"
