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
  int testNode;
  map<int,edge*> adjacentEdges;
  queue<message*> messageQueue;
  mutex shareMutex;

  //-----------------Parameterized Constructor------------------
  Node(int nodeId)
  {
    this->state=SLEEP;
    this->level=-1;
    this->name="";
    this->parent=NULL;
    this->bestWeight=INT_MAX;
    this->bestNode=NULL;
    this->rec=-1;
    this->testNode=-1;
    this->nodeId=nodeId;
  }



  void pushMessage(message msg)
  {
    message* message=&msg;
    this->shareMutex.lock();
    this->messageQueue.push(message);
    this->shareMutex.unlock();
  }

  int getKey(int wt)
  {
    for(auto i:this->adjacentEdges)
    {
      if(i.second->weight==wt)
        return i.first;
    }
    return -1;
  }

  //arguments[1] reserved for key of adjacentEdges
  void pullMessage()
  {
    if(!this->messageQueue.empty())
    {
      this->shareMutex.lock();
      message* newMessage = this->messageQueue.front();
      this->messageQueue.pop();
      int key = getKey(newMessage->arguments[1]);
      this->shareMutex.unlock();

      if(this->state==SLEEP)
        this->initialConnect();

      switch(newMessage->message)
      {
        case START:
          cout<<"Start:"<<this->nodeId<<" "<<key<<endl;
          initialConnect();
          break;
        case CONNECT:
          //1st argument = level, 2nd argument = key
          cout<<"Connect:"<<this->nodeId<<" "<<key<<endl;
          processConnectRequest(newMessage->arguments[0],key);
          break;
        case INITIATE:
          //1st argument = level, 2nd argument = state, 3rd argument = key, 4th argument = name
          cout<<"Initiate:"<<this->nodeId<<" "<<key<<endl;
          processInitiateRequest(newMessage->arguments[0],newMessage->state,key,newMessage->name);
          break;
        case TEST:
          //1st argument = level, 2nd argument = name, third argument = key
          cout<<"Test:"<<this->nodeId<<" "<<key<<endl;
          processTestRequest(newMessage->arguments[0],newMessage->name,key);
          break;
        case REJECT:
          //1st argument = key
          cout<<"Reject:"<<this->nodeId<<" "<<key<<endl;
          processRejectRequest(key);
          break;
        case ACCEPT:
          //1st argument = key
          cout<<"Accept:"<<this->nodeId<<" "<<key<<endl;
          processAcceptRequest(key);
          break;
        case REPORT:
        //1st argument = bestweight, 2nd argument = key
          cout<<"Report:"<<this->nodeId<<" "<<key<<endl;
          processReportRequest(newMessage->arguments[0],key);
          break;
        case CHANGEROOT:
          //1st argument = key
          cout<<"Changeroot:"<<this->nodeId<<" "<<key<<endl;
          processChangeRootRequest(key);
          break;
      }
    }
  }

  void initialConnect()
  {
    //---Finding Minimum Edge---
    int min=INT_MAX;
    int key=-1;
    for(auto i:this->adjacentEdges)
    {
      if(i.second->weight<min)
      {
        min=i.second->weight;
        key = i.first;
      }
    }
    //---------------------
    this->adjacentEdges[key]->state=BRANCH;
    this->state=FOUND;
    this->level=0;
    this->rec=0;

    addEdgeToMSTLock.lock();
    string outputString="";
    outputString+="("+to_string(this->nodeId)+", "+to_string(this->adjacentEdges[key]->destNode->nodeId)+", "+to_string(this->adjacentEdges[key]->weight)+")";
    mstEdges.push_back(outputString);
    addEdgeToMSTLock.unlock();
    //----Send Connect Message------
    message messageToSend;
    messageToSend.message = CONNECT;
    messageToSend.arguments[0] = 0;
    messageToSend.arguments[1] = min;
    this->adjacentEdges[key]->destNode->pushMessage(messageToSend);
  }

  void processConnectRequest(int levelOfNode,int key)
  {

    if(levelOfNode<this->level)
    {

      this->adjacentEdges[key]->state=BRANCH;
      message messageToSend;
      messageToSend.message=INITIATE;
      messageToSend.arguments[0]=this->level;
      messageToSend.arguments[1]=this->adjacentEdges[key]->weight;
      messageToSend.name=this->name;
      messageToSend.state=this->state;
      this->adjacentEdges[key]->destNode->pushMessage(messageToSend);
    }
    else if(this->adjacentEdges[key]->state==BASIC)
    {
      //----Wait: Push message back to queue--------
      message messageToSend;
      messageToSend.message=CONNECT;
      messageToSend.arguments[0]=levelOfNode;
      messageToSend.arguments[1]=this->adjacentEdges[key]->weight;
      this->pushMessage(messageToSend);
    }
    else
    {
      int temp = this->adjacentEdges[key]->weight;
      message messageToSend;
      messageToSend.message = INITIATE;
      messageToSend.arguments[0] = this->level+1;

      messageToSend.arguments[1] = this->adjacentEdges[key]->weight;

      string temp1 = to_string(temp);
      messageToSend.name=temp1;
      messageToSend.state=FIND;
      this->adjacentEdges[key]->destNode->pushMessage(messageToSend);
    }
  }

  void processInitiateRequest(int levelOfNode,nodeState stateOfNode, int key, string nameOfFragment)
  {
    //-------Set Values---------
    this->level = levelOfNode;
    this->state = stateOfNode;
    this->name = nameOfFragment;
    this->parent = this->adjacentEdges[key]->destNode;
    this->bestNode = NULL;
    this->bestWeight = INT_MAX;

    //--------Sending initiate to all neighbours-------
    for(auto i:this->adjacentEdges)
    {
      if(i.second->state==BRANCH && i.first!=this->parent->nodeId)
      {
        message messageToSend;
        messageToSend.message = INITIATE;
        messageToSend.arguments[0] = this->level;
        messageToSend.arguments[1] = i.second->weight;
        messageToSend.name=this->name;
        messageToSend.state=this->state;
        this->adjacentEdges[i.first]->destNode->pushMessage(messageToSend);
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
    int key=-1,min=INT_MAX;
    for(auto i:this->adjacentEdges)
    {
      if(i.second->state==BASIC && i.second->weight<min)
      {
        min=i.second->weight;
        key=i.first;
      }
    }
    if(key>=0)
    {
      this->testNode = key;
      message messageToSend;
      messageToSend.message = TEST;
      messageToSend.arguments[0]=this->level;
      messageToSend.arguments[1]=this->adjacentEdges[key]->weight;
      messageToSend.name=this->name;
      this->adjacentEdges[this->testNode]->destNode->pushMessage(messageToSend);
    }
    else
    {
      this->testNode=-1;
      report();
    }
  }

  void processTestRequest(int levelOfNode,string nameOfFragment, int key)
  {
    if(levelOfNode>this->level)
    {
      message messageToSend;
      messageToSend.message=TEST;
      messageToSend.arguments[0]=levelOfNode;
      messageToSend.arguments[1]=this->adjacentEdges[key]->weight;
      messageToSend.name=nameOfFragment;
      this->pushMessage(messageToSend);
    }
    else if(nameOfFragment.compare(this->name)==0)
    {

      if(this->adjacentEdges[key]->state==BASIC)
        this->adjacentEdges[key]->state=REJECTE;
      if(this->testNode!=this->adjacentEdges[key]->destNode->nodeId)
      {
        message messageToSend;
        messageToSend.message=REJECT;
        messageToSend.arguments[0] = -1;
        messageToSend.arguments[1] = this->adjacentEdges[key]->weight;
        this->adjacentEdges[key]->destNode->pushMessage(messageToSend);
      }
      else
        testPhase();
    }
    else
    {
      message messageToSend;
      messageToSend.message=ACCEPT;
      messageToSend.arguments[0] = -1;
      messageToSend.arguments[1] = this->adjacentEdges[key]->weight;
      this->adjacentEdges[key]->destNode->pushMessage(messageToSend);
    }
  }

  void processAcceptRequest(int key)
  {
    this->testNode=-1;
    if(this->adjacentEdges[key]->weight<this->bestWeight)
    {
      this->bestWeight=this->adjacentEdges[key]->weight;
      this->bestNode=this->adjacentEdges[key]->destNode;
    }
    report();
  }

  void processRejectRequest(int key)
  {
    if(this->adjacentEdges[key]->state==BASIC)
      this->adjacentEdges[key]->state=REJECTE;
    testPhase();
  }

  void report()
  {
    int count=0;
    for(auto i:this->adjacentEdges)
      if(i.second->state==BRANCH && this->parent->nodeId!=i.second->destNode->nodeId)
        count++;

    if(count==this->rec && this->testNode==-1)
    {
      this->state=FOUND;
      message messageToSend;
      messageToSend.message=REPORT;
      messageToSend.arguments[0]=this->bestWeight;
      messageToSend.arguments[1]=this->adjacentEdges[this->parent->nodeId]->weight;//Can cause segmentation
      this->parent->pushMessage(messageToSend);
    }
  }

  void processReportRequest(int bestWt, int key)
  {
    if(this->adjacentEdges[key]->destNode->nodeId!=this->parent->nodeId)
    {
      cout<<"Condition1"<<endl;
      if(bestWt<this->bestWeight)
      {
        this->bestWeight=bestWt;
        this->bestNode=this->adjacentEdges[key]->destNode;
      }
      this->rec=this->rec+1;
      report();
      cout<<"Condition1End"<<endl;
    }
    else
    {
      if(this->state==FIND)
      {
        //cout<<"Condition2"<<" "<<key<<" "<<this->nodeId<<endl;
        message messageToSend;
        messageToSend.message=REPORT;
        messageToSend.arguments[0]=bestWt;
        messageToSend.arguments[1]=this->adjacentEdges[key]->weight;
        this->pushMessage(messageToSend);
        //cout<<"Condition2End"<<endl;
      }
      else if(bestWt>this->bestWeight)
      {
        //cout<<"Condition3"<<endl;
        changeRoot();
        //cout<<"Condition3End"<<endl;
      }
      else if(this->bestWeight==INT_MAX && bestWt==INT_MAX)
      {
        //cout<<"Condition4"<<endl;
        stopFlag=1;
        //cout<<"Condition4"<<endl;
      }
    }
  }

  void changeRoot()
  {
    if(this->adjacentEdges[this->bestNode->nodeId]->state==BRANCH)
    {
      message messageToSend;
      messageToSend.message=CHANGEROOT;
      messageToSend.arguments[0]=-1;
      messageToSend.arguments[1]=this->adjacentEdges[this->bestNode->nodeId]->weight;
      this->bestNode->pushMessage(messageToSend);
    }
    else
    {
      this->adjacentEdges[this->bestNode->nodeId]->state=BRANCH;
      message messageToSend;
      messageToSend.message=CONNECT;
      messageToSend.arguments[0]=this->level;
      messageToSend.arguments[1]=this->adjacentEdges[this->bestNode->nodeId]->weight;
      this->bestNode->pushMessage(messageToSend);
      addEdgeToMSTLock.lock();
      int ind = this->bestNode->nodeId;
      int index=ind;
      //cout<<"Pushing : "<<this->nodeId<<" "<<this->adjacentEdges[ind]->destNode->nodeId<<endl;
      string outputString="";
      outputString+="("+to_string(this->nodeId)+", "+to_string(this->adjacentEdges[index]->destNode->nodeId)+", "+to_string(this->adjacentEdges[index]->weight)+")";
      mstEdges.push_back(outputString);
      addEdgeToMSTLock.unlock();
    }
  }

  void processChangeRootRequest(int key)
  {
    changeRoot();
  }
};
