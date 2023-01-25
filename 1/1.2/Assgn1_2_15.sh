#!/bin/bash
while IFS= read -r u; do
    echo $u|grep -qP "(?=^[a-zA-Z][a-zA-Z0-9]*[0-9][a-zA-Z0-9]*$)(?=^.{5,20}$)" && awk '{if(!(s+0) && index(tolower(var),tolower($1))){print"NO";s=1}};END{if(!(s+0))print"YES"}' var="$u" fruits.txt || echo "NO"
done <"$1">../validation_results.txt