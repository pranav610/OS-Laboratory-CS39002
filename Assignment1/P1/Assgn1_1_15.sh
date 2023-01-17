#! /bin/bash 

function gcd()
{
    a=$1
    b=$2
    while [ $b -gt 0 ]
    do
        temp=$a
        a=$b
        b=$(( temp%b ))
    done
    echo $a
}
function lcm()
{
    a=$1
    b=$2
    d=$( gcd $a $b )
    echo $(( a*(b/d) ))
}
echo -n "The reversed numbers are ">> output.txt
rev ./lcm.txt 1> rev_num.txt
file="rev_num.txt"
LCM=$(head -n 1 $file)
count=0
while read str
do
    if [ "$str" != "" ]
    then
            echo -n " ,$str" >> output.txt
            LCM=$( lcm $LCM $str )
    fi
done < $file
echo "" >> output.txt 
echo "LCM for the above numbers is : $LCM" >> output.txt