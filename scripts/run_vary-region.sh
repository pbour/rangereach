for d in foursquare gowalla weeplaces yelp
do
    echo $d
    for m in spareach-bfl spareach-int socreach 3dreach 3dreach_rev
    do
        for r in 0.01 0.02 0.05 0.1 0.2
        do
            echo "./run_${m}.exec ./inputs/${d}/${d} ./queries/${d}/queries-range-degree.${r}-2.txt.qry > ./outputs/${d}/run_${m}_queries-range-degree.${r}-2.txt"
            ./run_${m}.exec ./inputs/${d}/${d} ./queries/${d}/queries-range-degree.${r}-2.txt.qry > ./outputs/${d}/run_${m}_queries-range-degree.${r}-2.txt
        done
        echo
    done

    for m in spareach-int_MBR
    do
        for r in 0.01 0.02 0.05 0.1 0.2
        do
            echo "./run_${m}.exec ./inputs/${d}/${d} ./queries/${d}/queries-range-degree.${r}-2.txt.qry > ./outputs/${d}/run_${m}_queries-range-degree.${r}-2.txt"
            ./run_${m}.exec ./inputs/${d}/${d} ./queries/${d}/queries-range-degree.${r}-2.txt.qry > ./outputs/${d}/run_${m}_queries-range-degree.${r}-2.txt
        done
        echo
    done
    echo
done

