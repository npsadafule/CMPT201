#!/bin/bash
#Function to add two numbers

sum(){
  local sum=$(( $1 + $2 ))
  echo "Sum: $sum"
}

echo "Enter two numbers: "
read a b
sum $a $b
