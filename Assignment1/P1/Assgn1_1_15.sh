LCM=1
rev ./lcm.txt | while read str; do
    if [[ $str == "" ]];then
    continue;
    fi
    a_temp=$LCM
    b_temp=$str
    while [ $b_temp -gt 0 ];
    do
        temp=$a_temp
        a_temp=$b_temp
        b_temp=$(( temp%b_temp ))
    done
    LCM=$(( $LCM*($str/a_temp) ))
    echo $LCM > output.txt
done
