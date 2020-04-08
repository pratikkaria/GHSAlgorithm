#include<bits/stdc++.h>
using namespace std;
#include "headers.h"

class Node
{
public:
  nodeState state;
  string name;
  int level,bestWeight,rec,nodeId;
  Node* parent;
  Node* bestNode;
  Node* testNode;
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
    this->parent=NULL;
    this->bestWeight=INT_MAX;
    this->bestNode=NULL;
    this->rec=-1;
    this->testNode=NULL;
    this->nodeId=nodeId;
  }

  //--------------------Helper Functions----------------------
  int getIndex(int weight)
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
    return index;
  }

  edge* findEdgeTo(Node* neighbour)
  {
    for(int i=0;i<this->adjacentEdges.size();i++)
    {
      if(this->adjacentEdges[i]->first->nodeId==neighbour->nodeId || this->adjacentEdges[i]->second->nodeId==neighbour->nodeId)
        return this->adjacentEdges[i];
    }
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
    messageToSend->state=SLEEP;

    if(getValue.first->first->nodeId==this->nodeId)
      getValue.first->second->sendMessage(messageToSend);
    else
      getValue.first->first->sendMessage(messageToSend);

  }

  //--------------------Process incoming connect request-------------------
  void processConnectRequest(int level,int weight)
  {
    int index = getIndex(weight);
    if(index==-1)
      return;
    if(level<this->level)
    {
      this->adjacentEdges[index]->state=BRANCH;
      message* messageToSend;
      messageToSend->message=INITIATE;
      messageToSend->arguments[0]=this->level;
      messageToSend->arguments[1]=weight;
      messageToSend->name=this->name;
      messageToSend->state=this->state;

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

      messageToSend->state=FIND;
      messageToSend->arguments[1]=weight;

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
  void processInitiateRequest(int level,nodeState state,int weight,string name)
  {
    int index = getIndex(weight);
    if(index==-1)
      return;
    this->level = level;
    this->state = state;
    this->name = name;
    Node* temp;
    if(adjacentEdges[index]->first->nodeId==this->nodeId)
    {
      this->parent = adjacentEdges[index]->second;
      temp = adjacentEdges[index]->second;
    }
    else
    {
      this->parent = adjacentEdges[index]->first;
      temp = adjacentEdges[index]->first;
    }

    message* messageToSend;
    messageToSend->message=INITIATE;
    messageToSend->arguments[0]=level;
    messageToSend->arguments[1]=state;
    messageToSend->arguments[2]=weight;
    messageToSend->name=name;

    for(int i=0;i<neighbours.size();i++)
    {
      edge* edgeBetween = findEdgeTo(neighbours[i]);
      if(edgeBetween->state==BRANCH && temp->nodeId!=neighbours[i]->nodeId)
        neighbours[i]->sendMessage(messageToSend);
    }

    if(state==FIND)
    {
      this->rec=0;
      this->testPhase(weight);
    }
  }

  void testPhase(int weight)
  {
    pair<edge*,int> getMinEdge = findMinEdge();
    if(getMinEdge.second!=-1)
    {
      if(getMinEdge.first->first->nodeId==this->nodeId)
        this->testNode=getMinEdge.first->second;
      else
        this->testNode=getMinEdge.first->first;

      message* messageToSend;
      messageToSend->message=TEST;
      messageToSend->name=this->name;
      messageToSend->arguments[0]=this->level;
      messageToSend->arguments[1]=this->adjacentEdges[getMinEdge.second]->weight;
      this->testNode->sendMessage(messageToSend);
    }
    else
    {
      this->testNode=NULL;
      report(weight);
    }
  }


  //------------------------Process Test Message------------------
  void rejectEdge(int weight)
  {
    adjacentEdges[getIndex(weight)]->state=REJECTE;
  }

  void processTestRequest(int level,string name,int weight)
  {
    edge* edgeBetween=adjacentEdges[getIndex(weight)];
    Node* node1;
    if(edgeBetween->first->nodeId==this->nodeId)
      node1 = edgeBetween->second;
    else
      node1 = edgeBetween->first;
    if(level>this->level)
      //wait
      return;
    else if(name==this->name)
    {
      if(edgeBetween->state==BASIC)
      {
        adjacentEdges[getIndex(weight)]->state=REJECTE;
        node1->rejectEdge(weight);
      }
      if(node1->nodeId!=testNode->nodeId)
      {
        message* messageToSend;
        messageToSend->message=REJECT;
        messageToSend->arguments[0]=weight;
        node1->sendMessage(messageToSend);
      }
      else
        testPhase(weight);
    }
    else
    {
      message* messageToSend;
      messageToSend->message=ACCEPT;
      messageToSend->arguments[0]=weight;
      node1->sendMessage(messageToSend);
    }
  }

  //------------------------Process Reject Request------------------
  void processRejectRequest(int weight)
  {
    int ind = getIndex(weight);
    if(adjacentEdges[ind]->state==BASIC)
      adjacentEdges[ind]->state=REJECTE;

    this->testPhase(weight);
  }

  //------------------------Process Accept Request------------------
  void processAcceptRequest(int weight)
  {
    this->testNode=NULL;
    if(weight<this->bestWeight)
    {
      this->bestWeight=weight;
      int ind = getIndex(weight);
      if(adjacentEdges[ind]->first->nodeId==this->nodeId)
        this->parent=adjacentEdges[ind]->second;
      else
        this->parent=adjacentEdges[ind]->first;
    }
    this->report(weight);
  }


  //-------------------------Report Function------------------------
  void report(int weight)
  {
    int count = 0;
    for(int i=0;i<adjacentEdges.size();i++)
    {
      Node* temp;
      if(adjacentEdges[i]->first->nodeId==this->nodeId)
        temp=adjacentEdges[i]->second;
      else
        temp=adjacentEdges[i]->first;

      if(adjacentEdges[i]->state==BRANCH && temp->nodeId!=this->parent->nodeId)
        count++;
    }
    if(this->rec==count && this->testNode==NULL)
    {
      this->state=FOUND;
      message* messageToSend;
      messageToSend->message=REPORT;
      messageToSend->arguments[0]=this->bestWeight;
      messageToSend->arguments[1]=weight;
      this->parent->sendMessage(messageToSend);
    }
  }

  //-------------------------Process Report Request-----------------
  void processReportRequest(int bestWt,int weight)
  {
    Node* q;
    int ind = getIndex(weight);
    if(adjacentEdges[ind]->first->nodeId==this->nodeId)
      q = adjacentEdges[ind]->second;
    else
      q = adjacentEdges[ind]->first;

    if(this->parent->nodeId!=q->nodeId)
    {
      if(bestWt<this->bestWeight)
      {
        this->bestWeight=bestWt;
        this->bestNode=q;
      }
      this->rec+=1;
      this->report(weight);
    }
    else
    {
      if(this->state==FIND)
      {
        return;
        //Wait
      }
      else if(bestWt>this->bestWeight)
        this->changeRoot(weight);
      else if(bestWt==this->bestWeight && bestWt==INT_MAX)
      {
        return;
        //Stop
      }
    }
  }

  //------------------------Change Root Function--------------------
  void changeRoot(int weight)
  {
    edge* best;
    int ind=-1;
    for(int i=0;i<adjacentEdges.size();i++)
    {
      if(adjacentEdges[i]->first->nodeId==this->bestNode->nodeId || adjacentEdges[i]->second->nodeId==this->bestNode->nodeId)
      {
        best=adjacentEdges[i];
        ind=i;
        break;
      }
    }

    if(best->state==BRANCH)
    {
      message* messageToSend;
      messageToSend->message=CHANGEROOT;
      messageToSend->arguments[0]=weight;
      this->bestNode->sendMessage(messageToSend);
    }
    else
    {
      adjacentEdges[ind]->state=BRANCH;
      message* messageToSend;
      messageToSend->message=CONNECT;
      messageToSend->arguments[0]=this->level;
      this->bestNode->sendMessage(messageToSend);
    }
  }

  //------------------------Process Change root message-------------
  void processChangeRootRequest(int weight)
  {
    changeRoot(weight);
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
        processInitiateRequest(msg->arguments[0],msg->state,msg->arguments[1],msg->name);
        cout<<"initiate"<<endl;
        break;
      case TEST:
        processTestRequest(msg->arguments[0],msg->name,msg->arguments[1]);
        cout<<"test"<<endl;
        break;
      case REJECT:
        processRejectRequest(msg->arguments[0]);
        cout<<"reject"<<endl;
        break;
      case ACCEPT:
        processAcceptRequest(msg->arguments[0]);
        cout<<"accept"<<endl;
        break;
      case REPORT:
        processReportRequest(msg->arguments[0],msg->arguments[1]);
        cout<<"report"<<endl;
        break;
      case CHANGEROOT:
        processChangeRootRequest(msg->arguments[0]);
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
    else
    {
      retPair.second=-1;
    }
    return retPair;
  }
};


int main()
{
  Node temp(4);
  temp.processInitiateRequest(4,FIND,5,"hello");
  pair<edge*,int> retPair;
  retPair.second=-1;
  cout<<retPair.second<<endl;
  return 0;
}
