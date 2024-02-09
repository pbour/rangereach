for d in foursquare gowalla weeplaces yelp
do
    echo $d
    for m in spareach-bfl spareach-int socreach 3dreach 3dreach_rev
    do
        for de in 0 1 2 3 4
        do
                echo "./run_${m}.exec ./inputs/${d}/${d} ./queries/${d}/queries-range-degree.0.05-${de}.txt.qry > ./outputs/${d}/run_${m}_queries-range-degree.0.05-${de}.txt"
                ./run_${m}.exec ./inputs/${d}/${d} ./queries/${d}/queries-range-degree.0.05-${de}.txt.qry > ./outputs/${d}/run_${m}_queries-range-degree.0.05-${de}.txt
        done
        echo
    done

    for m in spareach-int_MBR
    do
        for de in 0 1 2 3 4
        do
                echo "./run_${m}.exec ./inputs/${d}/${d} ./queries/${d}/queries-range-degree.0.05-${de}.txt.qry > ./outputs/${d}/run_${m}_queries-range-degree.0.05-${de}.txt"
                ./run_${m}.exec ./inputs/${d}/${d} ./queries/${d}/queries-range-degree.0.05-${de}.txt.qry > ./outputs/${d}/run_${m}_queries-range-degree.0.05-${de}.txt
        done
        echo
    done
    echo
done
