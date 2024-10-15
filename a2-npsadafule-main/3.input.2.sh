#!/bin/bash
#Script to read from STDIN and print line 3 only

count=0
while IFS= read -r line; do
  count=$((count + 1))
  if [ "$count" -eq 3 ]; then
    echo "$line"
  fi
done < /dev/stdin
