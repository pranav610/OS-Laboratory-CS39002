#!/bin/bash

awk '{print$2}' ./$1|sort -n|uniq -c|sort -k1rn -k2|awk '{print$2"\t"$1}'
awk '{if(!($1 in seen)){seen[$1];i=i+1}else{print$1;i=i-1}}; END{print i+0}' ./$1
