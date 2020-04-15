import os
import random
import sys

startNode=50
ghsExecutable = sys.argv[1]
for i in range(50,101):
    outputLogFile = open("logFile","a")
    outputLogFile.write("\n\nNumber of Nodes:"+str(i)+"\n")
    outputLogFile.close();
    edgeCount=[]

    edgeCount.append(i-1);
    #edgeCount.append(int((i*(i-1))/2))
    edgeCount = random.sample(range(i, int((i*(i-1))/2)), 10)
    edgeCount.append(int((i*(i-1))/2))
    nodeMessageCount=0
    for j in edgeCount:
        numberOfNodes = i
        numberOfEdges = j
        commandToRun = "sh testYourImplementation.sh "+ghsExecutable+" primsMST "+str(i)+" "+str(j)+" > result.txt"
        os.system(commandToRun)
        fp = open("result.txt")
        result = fp.readlines()
        outputLogFile = open("logFile","a")
        fp = open("messageCount.txt")
        totalMessageCount = fp.readlines();
        totalMessageCount = totalMessageCount[0].split(":")[1]
        nodeMessageCount+=int(totalMessageCount)
        if result[1].split(" ")[0]=="Success":
            outputLogFile.write("Number of Nodes:"+str(i)+" Number of Edges: "+str(j)+" is Successfully Verified\n"+result[0])
            outputLogFile.write("Total Number of messages exchanged : "+str(totalMessageCount)+"\n")
            os.system("rm messageCount.txt")
        else:
            outputLogFile.write("Number of Nodes:"+str(i)+" Number of Edges: "+str(j)+" Failed\n")
        os.system("rm result.txt")
        outputLogFile.close();
    averageCount=nodeMessageCount/12;
    fp = open("average.txt","a")
    fp.write(str(i)+":"+str(averageCount)+"\n")
    fp.close()
