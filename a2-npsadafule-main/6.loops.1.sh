#!/bin/bash
#Script to print numbers 1-10 and whether they are even or odd

for number in {1..10}
do
  if (( $number % 2 == 0 ));then
    echo "$number, even"
  else
    echo "$number, odd"
  fi
done
