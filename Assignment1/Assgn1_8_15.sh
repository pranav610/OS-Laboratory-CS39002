#!/bin/bash
[ ! -f "main.csv" ] && echo "date,category,amount,name" > main.csv

while getopts ":c:n:s:h" opt; do
  case $opt in
    c) flagc=true;category=$OPTARG;;
    n) flagn=true;name=$OPTARG;;
    s) flags=true;[ "$OPTARG" = "date" ] && column_no=1;[ "$OPTARG" = "category" ] && column_no=2;[ "$OPTARG" = "amount" ] && column_no=3;[ "$OPTARG" = "name" ] && column_no=4;;
    h) echo "NAME\n\tExpense Tracker\n\nSYNOPSIS\n\tsh Assgn1_8_15.sh [OPTION]... ENTRY\n\nDESCRIPTION\n\tThis script tracks expenses by inserting new records into main.csv, and can also display total expenses by category or name, and sort the csv by a specified column.\n\n\t-c, --category\n\t\tDisplay total expenses by category.\n\n\t-n, --name\n\t\tDisplay total expenses by name.\n\n\t-s, --sort\n\t\tSort the csv by a specified column.\n\n\t-h, --help\n\t\tDisplay this help and exit.\n\nAUTHOR\n\tWritten by: Kulkarni Pranav Suryakant, Utsav Vinay Mehta, Swapnil Yasasvi, and Sidharth Vishwakarma.";exit;;
    \?) echo "Invalid option: -$OPTARG" >&2;exit;;
  esac
done

shift $((OPTIND-1))
echo "$1,$2,$3,$4" >> main.csv
echo "Inserted $1,$2,$3,$4 in main.csv"

header=$(head -1 main.csv)
tail -n +2 main.csv | sort -t, -k 1.7n -k 1.4,1.5n -k 1.1,1.2n > temp.csv

if [ "$flagc" = true ]; then
  awk -F, -v category="$category" 'BEGIN {sum=0} $2 == category {sum += $3} END {print sum}' main.csv
fi

if [ "$flagn" = true ]; then
  awk -F, -v name="$name" 'BEGIN {sum=0} $4 == name {sum += $3} END {print sum}' main.csv
fi

if [ "$flags" = true ]; then
  if [ "$column_no" = 3 ]; then
    tail -n +2 main.csv | sort -t, -k3 -n > temp.csv
  else
    tail -n +2 main.csv | sort -t, -k$column_no > temp.csv
  fi
fi

echo $header | cat - temp.csv > main.csv
rm temp.csv