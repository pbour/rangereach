for d in foursquare gowalla weeplaces yelp
do
    echo $d
    for m in spareach-bfl spareach-int socreach 3dreach 3dreach_rev
    do
        for s in 0.00001 0.0001 0.001 0.01
        do
                echo "./run_${m}.exec ./inputs/${d}/${d} ./queries/${d}/queries-selectivity.${s}.txt.qry > ./outputs/${d}/run_${m}_queries-selectivity.${s}.txt"
                ./run_${m}.exec ./inputs/${d}/${d} ./queries/${d}/queries-selectivity.${s}.txt.qry > ./outputs/${d}/run_${m}_queries-selectivity.${s}.txt
        done
        echo
    done

    for m in spareach-int_MBR
    do
        for s in 0.00001 0.0001 0.001 0.01
        do
                echo "./run_${m}.exec ./inputs/${d}/${d} ./queries/${d}/queries-selectivity.${s}.txt.qry > ./outputs/${d}/run_${m}_queries-selectivity.${s}.txt"
                ./run_${m}.exec ./inputs/${d}/${d} ./queries/${d}/queries-selectivity.${s}.txt.qry > ./outputs/${d}/run_${m}_queries-selectivity.${s}.txt
        done
        echo
    done
    echo
done
