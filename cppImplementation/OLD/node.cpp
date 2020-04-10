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
  map<int,edge*> adjacentEdges;
  queue<message*> messageQueue;
  mutex shareMutex;

  //-----------------Parameterized Constructor------------------
  Node(int nodeId)
  {
    this->state=SLEEP;
    this->level=-1;
    this->name=to_string(nodeId);
    this->parent=NULL;
    this->bestWeight=INT_MAX;
    this->bestNode=NULL;
    this->rec=-1;
    this->testNode=NULL;
    this->nodeId=nodeId;
  }

  // Node() {}
  //
  // ~Node()
  // {
  //   this->adjacentEdges.clear();
  // }

  int getIndex(int wt)
  {
    for(int i=0;i<this->adjacentEdges.size();i++)
    {
      if(this->adjacentEdges[i]->weight==wt)
        return i;
    }
    return -1;
  }

  edge* findEdgeTo(Node* neighbour)
  {
    edge* toReturn;
    for(int i=0;i<this->adjacentEdges.size();i++)
    {
      if(this->adjacentEdges[i]->destNode->nodeId==neighbour->nodeId)
        return this->adjacentEdges[i];
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
      message* msg = this->messageQueue.front();
      // cout<<"Msg Weight:"<<msg->arguments[1]<<endl;
      // cout<<"Msg ID:"<<msg->message<<endl;
      // cout<<"---------------------------------------------"<<endl;
      this->messageQueue.pop();
      this->shareMutex.unlock();

      if(this->state==SLEEP)
        this->initialConnect();

      // addEdgeToMSTLock.lock();
      // cout<<"--------------------"<<endl;
      // printMST();
      // cout<<"--------------------"<<endl;
      // addEdgeToMSTLock.unlock();
      switch(msg->message)
      {
        case START:
          initialConnect();
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
    for(int i=0;i<this->adjacentEdges.size();i++)
    {
      if(this->adjacentEdges[i]->weight<min && this->adjacentEdges[i]->state==BASIC)
      {
        min=this->adjacentEdges[i]->weight;
        index=i;
        minedge=this->adjacentEdges[i];
      }
    }

    pair<edge*,int> toRet = make_pair(minedge,index);

    return toRet;
  }

  void initialConnect()
  {
    int index=-1;
    int min=INT_MAX;
    for(int i=0;i<this->adjacentEdges.size();i++)
    {
      if(this->adjacentEdges[i]->weight<min)
      {
        min=this->adjacentEdges[i]->weight;
        index=i;
      }
    }
    this->adjacentEdges[index]->state=BRANCH;
    addEdgeToMSTLock.lock();
    string outputString="";
    outputString+="("+to_string(this->nodeId)+", "+to_string(this->adjacentEdges[index]->destNode->nodeId)+", "+to_string(this->adjacentEdges[index]->weight)+")";
    mstEdges.push_back(outputString);
    addEdgeToMSTLock.unlock();


    this->state=FOUND;
    this->level=0;
    this->rec=0;

    message* messageToSend=(message*)malloc(sizeof(message));

    messageToSend->message=CONNECT;
    messageToSend->arguments[0]=this->level;
    messageToSend->arguments[1]=this->adjacentEdges[index]->weight;

    this->adjacentEdges[index]->destNode->sendMessage(messageToSend);

  }

  void processConnectRequest(int l,int wt)
  {
    int index = getIndex(wt);

    if(l<this->level)
    {
      this->adjacentEdges[index]->state=BRANCH;
      message* messageToSend=(message*)malloc(sizeof(message));
      messageToSend->message=INITIATE;
      messageToSend->arguments[0]=this->level;
      messageToSend->arguments[1]=this->adjacentEdges[index]->weight;
      messageToSend->name=this->name;
      messageToSend->state=this->state;

      this->adjacentEdges[index]->destNode->sendMessage(messageToSend);
    }
    else if(this->adjacentEdges[index]->state==BASIC)
    {
      message* messageToSend=(message*)malloc(sizeof(message));
      messageToSend->message=CONNECT;
      messageToSend->arguments[0]=l;
      messageToSend->arguments[1]=wt;
      this->sendMessage(messageToSend);
    }
    else
    {

      message* messageToSend=(message*)malloc(sizeof(message));
      messageToSend->message=INITIATE;
      messageToSend->arguments[0]=this->level+1;

      messageToSend->state=FIND;
      messageToSend->arguments[1]=this->adjacentEdges[index]->weight;

      messageToSend->name=this->name;
      messageToSend->name+=this->adjacentEdges[index]->destNode->name;
      this->adjacentEdges[index]->destNode->sendMessage(messageToSend);
    }
  }

  void processInitiateRequest(int l,nodeState s,int wt,string n)
  {
    int index = getIndex(wt);

    this->level = l;
    this->state = s;
    this->name = n;
    Node* temp;
    this->parent = this->adjacentEdges[index]->destNode;
    this->bestNode=NULL;
    this->bestWeight=INT_MAX;
    this->testNode=NULL;


    for(int i=0;i<this->adjacentEdges.size();i++)
    {
      if(adjacentEdges[i]->state==BRANCH && i!=index)
      {
        message* messageToSend=(message*)malloc(sizeof(message));
        messageToSend->message=INITIATE;
        messageToSend->arguments[0]=l;
        messageToSend->state=s;
        messageToSend->arguments[1]=this->adjacentEdges[i]->weight;
        messageToSend->name=n;
        this->adjacentEdges[i]->destNode->sendMessage(messageToSend);
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
    if(getMinEdge.second>=0)
    {
      this->testNode = this->adjacentEdges[getMinEdge.second]->destNode;
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

  void processTestRequest(int l,string n,int wt)
  {
    int index=getIndex(wt);
    if(index==-1)
      return;
    if(l>this->level)
    {
      message* messageToSend=(message*)malloc(sizeof(message));
      messageToSend->message=TEST;
      messageToSend->arguments[0]=l;
      messageToSend->arguments[1]=this->adjacentEdges[index]->weight;
      messageToSend->name=name;
      this->sendMessage(messageToSend);
    }
    else if(n.compare(this->name)==0)
    {
      if(this->adjacentEdges[index]->state==BASIC)
        this->adjacentEdges[index]->state=REJECTE;
      if(this->adjacentEdges[index]->destNode->nodeId!=this->testNode->nodeId)
      {
        message* messageToSend=(message*)malloc(sizeof(message));
        messageToSend->message=REJECT;
        messageToSend->arguments[0]=this->adjacentEdges[index]->weight;
        this->adjacentEdges[index]->destNode->sendMessage(messageToSend);
      }
      else
        testPhase();
    }
    else
    {
      message* messageToSend=(message*)malloc(sizeof(message));
      messageToSend->message=ACCEPT;
      messageToSend->arguments[0]=this->adjacentEdges[index]->weight;
      this->adjacentEdges[index]->destNode->sendMessage(messageToSend);
    }
  }

  void processAcceptRequest(int wt)
  {
    this->testNode=NULL;
    if(wt<this->bestWeight)
    {
      this->bestWeight=wt;
      this->bestNode = this->adjacentEdges[getIndex(wt)]->destNode;
    }
    this->report();
  }

  void processRejectRequest(int weight)
  {
    int ind = getIndex(weight);
    if(this->adjacentEdges[ind]->state==BASIC)
      this->adjacentEdges[ind]->state=REJECTE;
    this->testPhase();
  }

  void report()
  {
    int count = 0;
    for(int i=0;i<this->adjacentEdges.size();i++)
    {
      Node* temp=this->adjacentEdges[i]->destNode;

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

  void processReportRequest(int bestWt,int wt)
  {
    int ind = getIndex(wt);
    Node* q = this->adjacentEdges[ind]->destNode;

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
        messageToSend->arguments[1]=this->adjacentEdges[ind]->weight;
        this->sendMessage(messageToSend);
      }
      else if(bestWt>this->bestWeight)
        changeRoot();
      else if(this->bestWeight==INT_MAX && bestWt==INT_MAX)
        stopFlag=1;
    }
  }

  void changeRoot()
  {
    edge* best;
    int ind=-1;
    for(int i=0;i<this->adjacentEdges.size();i++)
    {
      if(this->adjacentEdges[i]->destNode->nodeId==this->bestNode->nodeId)
      {
        best=this->adjacentEdges[i];
        ind=i;
        break;
      }
    }
    cout<<this->nodeId<<" "<<best->destNode->nodeId<<endl<<"-----------------"<<endl;
    if(best->state==BRANCH)
    {
      message* messageToSend=(message*)malloc(sizeof(message));
      messageToSend->message=CHANGEROOT;
      messageToSend->arguments[0]=this->adjacentEdges[ind]->weight;
      this->bestNode->sendMessage(messageToSend);
    }
    else
    {

      message* messageToSend=(message*)malloc(sizeof(message));
      messageToSend->message=CONNECT;
      messageToSend->arguments[0]=this->level;
      messageToSend->arguments[1]=this->adjacentEdges[ind]->weight;
      this->bestNode->sendMessage(messageToSend);
      this->adjacentEdges[ind]->state=BRANCH;
      int index=ind;
      addEdgeToMSTLock.lock();
      cout<<"Pushing : "<<this->nodeId<<" "<<adjacentEdges[ind]->destNode->nodeId<<endl;
      string outputString="";
      outputString+="("+to_string(this->nodeId)+", "+to_string(this->adjacentEdges[index]->destNode->nodeId)+", "+to_string(this->adjacentEdges[index]->weight)+")";
      mstEdges.push_back(outputString);
      addEdgeToMSTLock.unlock();
    }

  }

  void printMST()
  {
    for(int i=0;i<mstEdges.size();i++)
      cout<<mstEdges[i]<<endl;
  }
  void processChangeRootRequest(int wt)
  {
    changeRoot();
  }

};
