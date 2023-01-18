#!/bin/bash
[ ! -f "main.csv" ] && echo "date,category,amount,name" > main.csv

while getopts ":c:n:s:h" opt; do
  case $opt in
    c) flagc=true;category=$OPTARG;;
    n) flagn=true;name=$OPTARG;;
    s) flags=true;column_name=$OPTARG;;
    h) echo "NAME\n\tExpense Tracker\n\nSYNOPSIS\n\tsh Assgn1_8_15.sh [OPTION]... ENTRY\n\nDESCRIPTION\n\tThis script tracks expenses by inserting new records into main.csv, and can also display total expenses by category or name, and sort the csv by a specified column.\n\n\t-c, --category\n\t\tDisplay total expenses by category.\n\n\t-n, --name\n\t\tDisplay total expenses by name.\n\n\t-s, --sort\n\t\tSort the csv by a specified column.\n\n\t-h, --help\n\t\tDisplay this help and exit.\n\nAUTHOR\n\tWritten by: Kulkarni Pranav Suryakant, Utsav Vinay Mehta, Swapnil Yasasvi, and Sidharth Vishwakarma.";exit;;
    \?) echo "Invalid option: -$OPTARG" >&2;exit;;
  esac
done

shift $((OPTIND-1))
echo "$1,$2,$3,$4" >> main.csv

header=$(head -1 main.csv)
tail -n +2 main.csv | sort -t, -k $(head -1 main.csv | tr , '\n' | grep -n date | cut -f1 -d:) > temp.csv


if [ "$flagc" = true ]; then
  awk -F, -v category="$category" 'BEGIN {sum=0} $2 == category {sum += $3} END {print sum}' main.csv
fi

if [ "$flagn" = true ]; then
  awk -F, -v name="$name" 'BEGIN {sum=0} $4 == name {sum += $3} END {print sum}' main.csv
fi

if [ "$flags" = true ]; then
  header=$(head -1 main.csv)
  if [ "$column_name" = "amount" ]; then
    tail -n +2 main.csv | sort -t, -k $(head -1 main.csv | tr , '\n' | grep -n $column_name | cut -f1 -d:) -n > temp.csv
  else
    tail -n +2 main.csv | sort -t, -k $(head -1 main.csv | tr , '\n' | grep -n $column_name | cut -f1 -d:) > temp.csv
  fi
fi

echo $header | cat - temp.csv > main.csv
rm temp.csv