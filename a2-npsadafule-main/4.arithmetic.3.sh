#!/bin/bash
#Script to generate random number between 0 and 100"

l_lim=$1
u_lim=$2

if [ -z "$l_lim" ] || [ -z "$u_lim" ]; then
  echo "Enter lower and upper limits"
  exit 1
fi

random_number=$(( RANDOM % (u_lim - l_lim + 1) + l_lim ))

echo "Random number between $l_lim and $u_lim is $random_number"
