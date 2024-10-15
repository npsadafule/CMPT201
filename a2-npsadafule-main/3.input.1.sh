#!/bin/bash
#Script to ask user name, last name, address and then output them

read -p "Enter your last name: " lastname
read -p "Enter your first name: " firstname
read -p "Enter your aaaddress: " address

echo "$firstname $lastname, $address"
