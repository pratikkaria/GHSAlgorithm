#include<bits/stdc++.h>
using namespace std;
#include "headers.h"

class Node
{
  nodeState state;
  int level,parent,bestWeight,bestNode,rec,testNode,nodeId;
  vector<int> neighbours;
  vector<edge*> adjacentEdges;
  queue<message> messageQueue;
  mutex shareMutex;

public:
  Node(int nodeId,vector<edge*> edgeList)
  {
    this->state=SLEEP;
    this->level=0;
    this->parent=-1;
    this->bestWeight=-1;
    this->bestNode=-1;
    this->rec=-1;
    this->testNode=-1;
    this->nodeId=nodeId;

    for(int i=0;i<edgeList.size();i++)
    {
      edge* temp = edgeList[i];
      if(temp->first==nodeId)
      {
        this->neighbours.push_back(temp->second);
        this->adjacentEdges.push_back(temp);
      }
      else if(temp->second==nodeId)
      {
        this->neighbours.push_back(temp->first);
        this->adjacentEdges.push_back(temp);
      }
    }
  }
};
int main()
{
  return 0;
}
