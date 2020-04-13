import numpy as np
import os
import random


temp = open("outGraph.txt","r")
temp = temp.readlines()
weights = random.sample(range(1, pow(len(temp),2)), len(temp))
out_str=""
numnodes = -1
for index,i in enumerate(temp):
    line = i.strip().split(" ")
    if max(int(line[0]),int(line[1]))>numnodes:
        numnodes = max(int(line[0]),int(line[1]))
    out_str+= "("+line[0]+", "+line[1]+", "+str(weights[index])+")\n"


numnodes+=1
out_str=str(numnodes)+"\n"+out_str
out_str=out_str.strip()
os.system('rm outGraph.txt')
out = open("testGraph.txt","w")
out.write(out_str)
print("Saved in testGraph.txt")
# print(out_str)
