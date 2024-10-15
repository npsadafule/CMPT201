#!/bin/bash
#Script to print directory contents with file sizes and item counts for directories
if [ -z "$1" ]; then
  echo "Please enter a directory"
  exit 1
fi

for entry in "$1"/*
do
  if [ -f "$entry" ]; then
    filesize=$(stat -c%s "$entry")
    echo "$filesize"
  elif [ -d "$entry" ]; then
    itemcount=$(ls -1A "$entry" | wc -l)
    echo "$itemcount"
  fi
done
