#!/bin/bash
touch "$1"
while IFS= read -r u
do
  echo "$u"|grep -P "^(?=[a-zA-Z])(?=.*[0-9])(?=.{4,19})"
done < usernames.txt >> "$1"