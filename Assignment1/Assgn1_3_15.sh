#!/bin/bash

json_dir=$1
csv_dir=$2
shift 2

for file in "$json_dir"*.jsonl
do
  filename="${file%.*}"  
  csv_file="$csv_dir$(basename "$filename").csv"
  header=""
  for attribute in "${@}"
  do
    header="$header,\"$attribute\""
  done
  if [[ -n "$header" ]]; then
    header="${header:1}"
  fi
  echo "$header" > $csv_file
  cols=".$1" 
  for attribute in "${@:2}"; do cols="$cols, .$attribute"; done
  cols="$cols"
  jq -r "[$cols] | flatten | @csv" $file >> $csv_file
done
