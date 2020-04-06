#include<bits/stdc++.h>
using namespace std;
#include "headers.h"

class Node
{
public:
  nodeState state;
  string name;
  int level,parent,bestWeight,bestNode,rec,testNode,nodeId;
  vector<Node*> neighbours;
  vector<edge*> adjacentEdges;
  queue<message*> messageQueue;
  mutex shareMutex;

  //-----------------Parameterized Constructor------------------
  Node(int nodeId)
  {
    this->state=SLEEP;
    this->level=0;
    this->name=to_string(nodeId);
    this->parent=-1;
    this->bestWeight=-1;
    this->bestNode=-1;
    this->rec=-1;
    this->testNode=-1;
    this->nodeId=nodeId;
  }

  //---------------------Initial Connect Request-----------------------
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
    messageToSend->arguments[1]=getValue.first->weight;
    messageToSend->name="";

    if(getValue.first->first->nodeId==this->nodeId)
      getValue.first->second->sendMessage(messageToSend);
    else
      getValue.first->first->sendMessage(messageToSend);

  }

  //--------------------Process incoming connect request-------------------
  void processConnectRequest(int level,int weight)
  {
    int index=-1;
    for(int i=0;i<adjacentEdges.size();i++)
    {
      if(this->adjacentEdges[i]->weight==weight)
      {
        index=i;
        break;
      }
    }
    if(index==-1)
      return;
    if(level<this->level)
    {
      this->adjacentEdges[index]->state=BRANCH;
      message* messageToSend;
      messageToSend->message=INITIATE;
      messageToSend->arguments[0]=this->level;
      messageToSend->arguments[1]=this->state;
      messageToSend->arguments[2]=weight;
      messageToSend->name=this->name;

      if(this->adjacentEdges[index]->first->nodeId==this->nodeId)
        this->adjacentEdges[index]->second->sendMessage(messageToSend);
      else
        this->adjacentEdges[index]->first->sendMessage(messageToSend);
    }
    else if(this->adjacentEdges[index]->state==BASIC)
    {
      //Wait
    }
    else
    {
      message* messageToSend;
      messageToSend->message=INITIATE;
      messageToSend->arguments[0]=this->level;

      messageToSend->arguments[1]=FIND;
      messageToSend->arguments[2]=weight;

      if(this->adjacentEdges[index]->first->nodeId==this->nodeId)
      {
        messageToSend->name=this->adjacentEdges[index]->second->name+this->name;
        this->adjacentEdges[index]->second->sendMessage(messageToSend);
      }
      else
      {
        messageToSend->name=this->adjacentEdges[index]->first->name+this->name;
        this->adjacentEdges[index]->first->sendMessage(messageToSend);
      }

    }

  }

  //------------------------Process Initiate Message------------------
  void processInitiateRequest(int level,int state,int weight,string name)
  {
    
  }

  //------------------------sendMessage to a node-------------------
  void sendMessage(message* msg)
  {
    this->shareMutex.lock();
    this->messageQueue.push(msg);
    this->shareMutex.unlock();
  }

  //------------------------Read message from the top-----------------
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
        processConnectRequest(msg->arguments[0],msg->arguments[1]);
        cout<<"connect"<<endl;
        break;
      case INITIATE:
        processInitiateRequest(msg->arguments[0],msg->arguments[1],msg->arguments[2],msg->name);
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

  //----------------Find min weight outgoing edge---------------
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
  Node temp(4);
  temp.processInitiateRequest(4,FIND,5,"hello");
  return 0;
}
