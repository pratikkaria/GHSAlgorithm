#include<bits/stdc++.h>
using namespace std;
#include "headers.h"

class Node
{
public:
  nodeState state;
  int level,parent,bestWeight,bestNode,rec,testNode,nodeId;
  vector<Node*> neighbours;
  vector<edge*> adjacentEdges;
  queue<message*> messageQueue;
  mutex shareMutex;
  Node(int nodeId)
  {
    this->state=SLEEP;
    this->level=0;
    this->parent=-1;
    this->bestWeight=-1;
    this->bestNode=-1;
    this->rec=-1;
    this->testNode=-1;
    this->nodeId=nodeId;

    // for(int i=0;i<edgeList.size();i++)
    // {
    //   edge* temp = edgeList[i];
    //   if(temp->first==nodeId)
    //   {
    //     this->neighbours.push_back(temp->second);
    //     this->adjacentEdges.push_back(temp);
    //   }
    //   else if(temp->second==nodeId)
    //   {
    //     this->neighbours.push_back(temp->first);
    //     this->adjacentEdges.push_back(temp);
    //   }
    // }
  }
  void initialConnect()
  {
    pair<edge*,int> getValue = findMinEdge();

    if(!getValue.first)
      return;

    addEdgeToMSTLock.lock();
    mstEdges.push_back(getValue.first);
    addEdgeToMSTLock.unlock();

    this->adjacentEdges[getValue.second]->state=BRANCH;
    this->state=FOUND;
    this->level=0;
    this->rec=0;

    message* messageToSend;
    messageToSend->message=CONNECT;
    messageToSend->arguments[0]=this->level;

    if(getValue.first->first->nodeId==this->nodeId)
      getValue.first->first->sendMessage(messageToSend);
    else
      getValue.first->second->sendMessage(messageToSend);

  }
  void sendMessage(message* msg)
  {
    this->shareMutex.lock();
    this->messageQueue.push(msg);
    this->shareMutex.unlock();
  }
  void readMessageFromTop()
  {
    message* msg = messageQueue.front();
    messageQueue.pop();
    switch(msg->message)
    {
      case START:
        initialConnect();
        break;
      case CONNECT:
        cout<<"connect"<<endl;
        break;
      case INITIATE:
        cout<<"initiate"<<endl;
        break;
      case TEST:
        cout<<"test"<<endl;
        break;
      case REJECT:
        cout<<"reject"<<endl;
        break;
      case ACCEPT:
        cout<<"accept"<<endl;
        break;
      case REPORT:
        cout<<"report"<<endl;
        break;
      case CHANGEROOT:
        cout<<"changeroot"<<endl;
        break;
    }
  }
  pair<edge*,int> findMinEdge()
  {
    int min=INT_MAX;
    int index=-1;
    edge* minedge;
    for(int i=0;i<adjacentEdges.size();i++)
    {
      if(adjacentEdges[i]->weight<min && adjacentEdges[i]->state==BASIC)
      {
        min=adjacentEdges[i]->weight;
        index=i;
        minedge=adjacentEdges[i];
      }
    }
    pair<edge*,int> retPair;
    if(minedge)
    {
      minedge->state=BASIC;
      retPair=make_pair(minedge,index);
    }
    return retPair;
  }
};
int main()
{
  return 0;
}
