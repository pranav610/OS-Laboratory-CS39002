#!/bin/bash

awk '{print$2}' ./$1|sort -n|uniq -c|sort -k1rn -k2|awk '{print$2"\t"$1}'
awk '{print$1}' ./$1|awk '{if(!($0 in seen)){seen[$0];i=i+1}else{print$0;i=i-1}}; END{print i+0}'