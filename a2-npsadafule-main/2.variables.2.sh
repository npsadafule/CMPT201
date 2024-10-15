#!/bin/bash
#script to generate random word

word=$(shuf -n 1 /usr/share/dict/words)
echo "$word"
