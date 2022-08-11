for i in /proc/*; do
    if [ -d $i ]; then
        for j in $i/stat; do

            cat $j
           #echo $j
        done
    fi

done