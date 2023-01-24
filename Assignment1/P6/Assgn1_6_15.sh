declare -a is_prime # declare smallest prime factor array
for(( i=0 ; i <= 1000000 ; i++))
do
    is_prime[i]=1
done
is_prime[0]=0
is_prime[1]=0
for(( i=2 ; i <= 1000000 ; i++))
do
   if [[ ${is_prime[i]} == 1 ]];
   then
        for(( j=i*i ; j<= 1000000 ; j+=i ))
        do
            is_prime[j]=0
        done
   fi
done

inputfile="input.txt"
while IFS=$' \t\r\n' read -r line
do 
    n=$line
    for(( i=2 ; i<= 1000000 && i <= $n ; i++))
    do
        if [[ ${is_prime[i]} == 1 ]]
        then
            rem=$((n%i))
            if [[ $rem == 0 ]]
            then
                echo -n "$i " >> output.txt
            fi
        fi
    done
    echo "" >> output.txt
done < $inputfile
