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

  int getIndex(int weight)
  {
    for(int i=0;i<this->adjacentEdges.size();i++)
    {
      if(this->adjacentEdges[i]->weight==weight)
        return i;
    }
    return -1;
  }

  edge* findEdgeTo(Node* neighbour)
  {
    edge* toReturn;
    for(int i=0;i<this->adjacentEdges.size();i++)
    {
      if(this->adjacentEdges[i]->first->nodeId==neighbour->nodeId || this->adjacentEdges[i]->second->nodeId==neighbour->nodeId)
      {
        toReturn=this->adjacentEdges[i];
        break;
      }
    }
    return toReturn;
  }


  void sendMessage(message* msg)
  {
    this->shareMutex.lock();
    this->messageQueue.push(msg);
    this->shareMutex.unlock();
  }



  void readMessageFromTop()
  {
    if(!this->messageQueue.empty())
    {
      this->shareMutex.lock();
      message* msg = messageQueue.front();
      messageQueue.pop();
      this->shareMutex.unlock();
      if(this->state==SLEEP)
        this->initialConnect();

      addEdgeToMSTLock.lock();
      cout<<"--------------------"<<endl;
      printMST();
      cout<<"--------------------"<<endl;
      addEdgeToMSTLock.unlock();
      switch(msg->message)
      {
        case START:
          this->initialConnect();
          break;
        case CONNECT:
          processConnectRequest(msg->arguments[0],msg->arguments[1]);
          break;
        case INITIATE:
          processInitiateRequest(msg->arguments[0],msg->state,msg->arguments[1],msg->name);
          break;
        case TEST:
          processTestRequest(msg->arguments[0],msg->name,msg->arguments[1]);
          break;
        case REJECT:
          processRejectRequest(msg->arguments[0]);
          break;
        case ACCEPT:
          processAcceptRequest(msg->arguments[0]);
          break;
        case REPORT:
          processReportRequest(msg->arguments[0],msg->arguments[1]);
          break;
        case CHANGEROOT:
          processChangeRootRequest(msg->arguments[0]);
          break;
      }
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
      retPair=make_pair(minedge,index);
    }
    else
    {
      retPair=make_pair(minedge,index);
    }

    return retPair;
  }


  void initialConnect()
  {
    int min = INT_MAX, index = -1;
    for (int i = 0; i < adjacentEdges.size(); i++)
    {
      if (this->adjacentEdges[i]->weight < min)
      {
        min = this->adjacentEdges[i]->weight;
        index = i;
      }
    }

    this->adjacentEdges[index]->state=BRANCH;
    addEdgeToMSTLock.lock();
    mstEdges.push_back(this->adjacentEdges[index]);
    addEdgeToMSTLock.unlock();


    this->state=FOUND;
    this->level=0;
    this->rec=0;

    message* messageToSend=(message*)malloc(sizeof(message));

    messageToSend->message=CONNECT;
    messageToSend->arguments[0]=this->level;
    messageToSend->arguments[1]=this->adjacentEdges[index]->weight;


    if(this->adjacentEdges[index]->first->nodeId==this->nodeId)
      this->adjacentEdges[index]->second->sendMessage(messageToSend);
    else
      this->adjacentEdges[index]->first->sendMessage(messageToSend);
  }



  void processConnectRequest(int level,int weight)
  {
    int index = getIndex(weight);

    if(level<this->level)
    {
      this->adjacentEdges[index]->state=BRANCH;
      message* messageToSend=(message*)malloc(sizeof(message));
      messageToSend->message=INITIATE;
      messageToSend->arguments[0]=this->level;
      messageToSend->arguments[1]=this->adjacentEdges[index]->weight;
      messageToSend->name=this->name;
      messageToSend->state=this->state;

      if(this->adjacentEdges[index]->first->nodeId==this->nodeId)
        this->adjacentEdges[index]->second->sendMessage(messageToSend);
      else
        this->adjacentEdges[index]->first->sendMessage(messageToSend);
    }
    else if(this->adjacentEdges[index]->state==BASIC)
    {
      message* messageToSend=(message*)malloc(sizeof(message));
      messageToSend->message=CONNECT;
      messageToSend->arguments[0]=level;
      messageToSend->arguments[1]=weight;
      this->sendMessage(messageToSend);
    }
    else
    {

      message* messageToSend=(message*)malloc(sizeof(message));
      messageToSend->message=INITIATE;
      messageToSend->arguments[0]=this->level+1;

      messageToSend->state=FIND;
      messageToSend->arguments[1]=weight;

      if(this->adjacentEdges[index]->first->nodeId==this->nodeId)
      {
        messageToSend->name=this->adjacentEdges[index]->second->name;
        messageToSend->name+=this->name;

        this->adjacentEdges[index]->second->sendMessage(messageToSend);
      }
      else
      {
        messageToSend->name=this->adjacentEdges[index]->first->name;
        messageToSend->name+=this->name;
        this->adjacentEdges[index]->first->sendMessage(messageToSend);
      }

    }

  }


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

    this->bestNode=NULL;
    this->bestWeight=INT_MAX;
    this->testNode=NULL;



    for(int i=0;i<this->neighbours.size();i++)
    {
      edge* edgeBetween = findEdgeTo(this->neighbours[i]);
      if(edgeBetween->state==BRANCH && temp->nodeId!=this->neighbours[i]->nodeId)
      {
        message* messageToSend=(message*)malloc(sizeof(message));
        messageToSend->message=INITIATE;
        messageToSend->arguments[0]=level;
        messageToSend->state=state;
        messageToSend->arguments[1]=edgeBetween->weight;
        messageToSend->name=name;
        this->neighbours[i]->sendMessage(messageToSend);
      }
    }

    if(this->state==FIND)
    {
      this->rec=0;
      testPhase();
    }
  }

  void testPhase()
  {
    pair<edge*,int> getMinEdge = findMinEdge();
    if(getMinEdge.second!=-1)
    {
      if(getMinEdge.first->first->nodeId==this->nodeId)
        this->testNode=getMinEdge.first->second;
      else
        this->testNode=getMinEdge.first->first;

      message* messageToSend=(message*)malloc(sizeof(message));
      messageToSend->message=TEST;
      messageToSend->name=this->name;
      messageToSend->arguments[0]=this->level;
      messageToSend->arguments[1]=this->adjacentEdges[getMinEdge.second]->weight;
      this->testNode->sendMessage(messageToSend);
    }
    else
    {
      this->testNode=NULL;
      report();
    }
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
    {
      message* messageToSend=(message*)malloc(sizeof(message));
      messageToSend->message=TEST;
      messageToSend->arguments[0]=level;
      messageToSend->arguments[1]=edgeBetween->weight;
      messageToSend->name=name;
      this->sendMessage(messageToSend);
    }
    else if(name==this->name)
    {
      if(edgeBetween->state==BASIC)
        this->adjacentEdges[getIndex(weight)]->state=REJECTE;
      if(node1->nodeId!=this->testNode->nodeId)
      {
        message* messageToSend=(message*)malloc(sizeof(message));
        messageToSend->message=REJECT;
        messageToSend->arguments[0]=edgeBetween->weight;
        node1->sendMessage(messageToSend);
      }
      else
        testPhase();
    }
    else
    {
      message* messageToSend=(message*)malloc(sizeof(message));
      messageToSend->message=ACCEPT;
      messageToSend->arguments[0]=edgeBetween->weight;
      node1->sendMessage(messageToSend);
    }
  }


  void processAcceptRequest(int weight)
  {
    this->testNode=NULL;
    if(weight<this->bestWeight)
    {
      this->bestWeight=weight;

      int ind = getIndex(weight);
      if(adjacentEdges[ind]->first->nodeId==this->nodeId)
        this->bestNode=adjacentEdges[ind]->second;
      else
        this->bestNode=adjacentEdges[ind]->first;
    }
    this->report();
  }

  void processRejectRequest(int weight)
  {
    int ind = getIndex(weight);
    if(adjacentEdges[ind]->state==BASIC)
      adjacentEdges[ind]->state=REJECTE;

    this->testPhase();
  }

  void report()
  {
    int count = 0;
    for(int i=0;i<adjacentEdges.size();i++)
    {
      Node* temp;
      if(this->adjacentEdges[i]->first->nodeId==this->nodeId)
        temp=this->adjacentEdges[i]->second;
      else
        temp=this->adjacentEdges[i]->first;

      if(this->adjacentEdges[i]->state==BRANCH && temp->nodeId!=this->parent->nodeId)
        count++;
    }
    if(this->rec==count && this->testNode==NULL)
    {
      this->state=FOUND;
      message* messageToSend=(message*)malloc(sizeof(message));
      messageToSend->message=REPORT;
      messageToSend->arguments[0]=this->bestWeight;
      messageToSend->arguments[1]=findEdgeTo(this->parent)->weight;
      this->parent->sendMessage(messageToSend);
    }
  }


  void processReportRequest(int bestWt,int weight)
  {
    cout<<"start "<<this->nodeId<<endl;
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
      this->report();
    }
    else
    {
      if(this->state==FIND)
      {
        message* messageToSend=(message*)malloc(sizeof(message));
        messageToSend->message=REPORT;
        messageToSend->arguments[0]=bestWt;
        messageToSend->arguments[1]=weight;
        this->sendMessage(messageToSend);
      }
      else if(bestWt>this->bestWeight)
        this->changeRoot();
      else if(this->bestWeight==INT_MAX && bestWt==INT_MAX)
      {
        stopFlag=1;
      }
    }
    cout<<"end "<<this->nodeId<<endl;
  }


  void changeRoot()
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
      message* messageToSend=(message*)malloc(sizeof(message));
      messageToSend->message=CHANGEROOT;
      messageToSend->arguments[0]=best->weight;
      this->bestNode->sendMessage(messageToSend);
    }
    else
    {
      adjacentEdges[ind]->state=BRANCH;
      message* messageToSend=(message*)malloc(sizeof(message));
      messageToSend->message=CONNECT;
      messageToSend->arguments[0]=this->level;
      messageToSend->arguments[1]=adjacentEdges[ind]->weight;
      this->bestNode->sendMessage(messageToSend);
      addEdgeToMSTLock.lock();
      mstEdges.push_back(adjacentEdges[ind]);
      addEdgeToMSTLock.unlock();
    }

  }

  void printMST()
  {
    for(int i=0;i<mstEdges.size();i++)
      cout<<mstEdges[i]->first->nodeId<<" "<<mstEdges[i]->second->nodeId<<" "<<mstEdges[i]->weight<<endl;
  }
  void processChangeRootRequest(int weight)
  {
    //cout<<"processChangeRootRequest start"<<this->nodeId<<endl;
    changeRoot();
    //cout<<"processChangeRootRequest end"<<this->nodeId<<endl;
  }

};
