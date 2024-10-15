#!/bin/bash
#Script to multiply two numbers using let, expr, double parentheses

let res_let=$1*$2
echo "Using let: $res_let"

res_expr=$(expr $1 \* $2)
echo "Using expr: $res_expr"

res_dp=$(( $1 * $2 ))
echo "Using double parentheses: $res_dp"
