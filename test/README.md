## Testing Using GHS Test Tool

Requirements - C++ , Python
```
$ python GHSTest.py <your C++ executable>
```
A logFile will be generated with verification of each testcase with the output of Prims Algorithm.

Suppose you coded in some different language, then just modify the execution command in testYourImplementation.sh to the
required command for execution. 

Eg) Suppose you are using Java then change the line:

./$2 $4 > ghsOutput.txt to java $2 $4 > ghsOutput.txt

Note: The input to the algorithm will be of the format in sampleTestCase.txt and output should be displayed on console 
in the format of sampleOutput.txt.

If you want to change number of times testing is done on each count of nodes, then modify GHSTest.py
