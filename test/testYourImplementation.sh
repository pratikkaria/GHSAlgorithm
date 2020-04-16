if  [ $1 = "--help" ]; then
    echo "Usage:"
    echo "sh testYourImplementation.sh <ghs executable> <prims executable> <numberofnodes> <numberofedges>"
elif [ $1 = "1" ]; then
    res1=$(date +%s.%N)
    ./$2 $4 > ghsOutput.txt
    res2=$(date +%s.%N)
    dt=$(echo "$res2 - $res1" | bc)
    dd=$(echo "$dt/86400" | bc)
    dt2=$(echo "$dt-86400*$dd" | bc)
    dh=$(echo "$dt2/3600" | bc)
    dt3=$(echo "$dt2-3600*$dh" | bc)
    dm=$(echo "$dt3/60" | bc)
    ds=$(echo "$dt3-60*$dm" | bc)
    LC_NUMERIC=C printf "Total runtime: %d:%02d:%02d:%02.4f\n" $dd $dh $dm $ds
    echo "GHS"
    echo $LC_NUMERIC
    echo "----"

    res1=$(date +%s.%N)
    ./$3 $4 > primsOutput.txt
    res2=$(date +%s.%N)
    dt=$(echo "$res2 - $res1" | bc)
    dd=$(echo "$dt/86400" | bc)
    dt2=$(echo "$dt-86400*$dd" | bc)
    dh=$(echo "$dt2/3600" | bc)
    dt3=$(echo "$dt2-3600*$dh" | bc)
    dm=$(echo "$dt3/60" | bc)
    ds=$(echo "$dt3-60*$dm" | bc)
    LC_NUMERIC=C printf "Total runtime: %d:%02d:%02d:%02.4f\n" $dd $dh $dm $ds
    echo "Prims"
    echo $LC_NUMERIC
    echo "-----"

    cmp --silent ghsOutput.txt primsOutput.txt && echo 'Success ' || echo 'Failure '
else
  ./genGraph $3 $4 > outGraph.txt
  python testGHS.py
  # ./../Final/findMST testGraph.txt > ghsOutput.txt
  # ./../Final/Prims/primsMST testGraph.txt > primsOutput.txt
  res1=$(date +%s.%N)
  ./$1 testGraph.txt > ghsOutput.txt
  res2=$(date +%s.%N)
  ./$2 testGraph.txt > primsOutput.txt
  dt=$(echo "$res2 - $res1" | bc)
  dd=$(echo "$dt/86400" | bc)
  dt2=$(echo "$dt-86400*$dd" | bc)
  dh=$(echo "$dt2/3600" | bc)
  dt3=$(echo "$dt2-3600*$dh" | bc)
  dm=$(echo "$dt3/60" | bc)
  ds=$(echo "$dt3-60*$dm" | bc)

  LC_NUMERIC=C printf "Total runtime: %d:%02d:%02d:%02.4f\n" $dd $dh $dm $ds
  runtime=$((end-start))
  cmp --silent ghsOutput.txt primsOutput.txt && echo 'Success '+$LC_NUMERIC || echo 'Failure '
  rm primsOutput.txt ghsOutput.txt testGraph.txt

fi
