#!/bin/bash
#Script to generate random number between 0 and 100"

random_number=$(( RANDOM % 101 ))
echo "Random number between 0 and 100: $random_number"
