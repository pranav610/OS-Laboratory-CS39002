#!/bin/bash
[ ! -f "main.csv" ] && echo "Date,Category,Amount,Name" > main.csv

while getopts ":c:n:s:h" opt; do
  case $opt in
    c) flagc=true;category=$OPTARG;;
    n) flagn=true;name=$OPTARG;;
    s) flags=true;column_name=$OPTARG;;
    h) echo "Utility Name: expense_tracker\nUsage: sh expense_tracker.sh [-c category] [-n name] [-s column] [-h] [date] [category] [amount] [name]\nDescription: This script tracks expenses by inserting new records into main.csv, and can also display total expenses by category or name, and sort the csv by a specified column.";exit;;
    \?) echo "Invalid option: -$OPTARG" >&2;exit;;
  esac
done

shift $((OPTIND-1))
echo "$1,$2,$3,$4" >> main.csv

if [ "$flagc" = true ]; then
  awk -F, -v category="$category" '$2 == category {sum += $3} END {print sum}' main.csv
fi

if [ "$flagn" = true ]; then
  awk -F, -v name="$name" '$4 == name {sum += $3} END {print sum}' main.csv
fi

if [ "$flags" = true ]; then
  header=$(head -1 main.csv)
  tail -n +2 main.csv | sort -t, -k $(head -1 main.csv | tr , '\n' | grep -n $column_name | cut -f1 -d:) > temp.csv
  echo $header | cat - temp.csv > main.csv
  rm temp.csv
fi
