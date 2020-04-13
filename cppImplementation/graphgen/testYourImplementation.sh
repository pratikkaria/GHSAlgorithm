if  [ $1 = "--help" ]; then
    echo "Usage:"
    echo "sh testYourImplementation.sh <ghs executable> <prims executable> <numberofnodes> <numberofedges>"
else
  ./genGraph $3 $4 > outGraph.txt
  python testGHS.py
  # ./../Final/findMST testGraph.txt > ghsOutput.txt
  # ./../Final/Prims/primsMST testGraph.txt > primsOutput.txt

  ./$1 testGraph.txt > ghsOutput.txt
  ./$2 testGraph.txt > primsOutput.txt

  cmp --silent ghsOutput.txt primsOutput.txt && echo '### SUCCESS: Files Are Identical! ###' || echo '### WARNING: Files Are Different! ###'
  rm primsOutput.txt ghsOutput.txt testGraph.txt

fi
