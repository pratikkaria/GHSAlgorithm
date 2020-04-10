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
    //cout<<"Constructor Start : "<<nodeId<<endl;
    this->state=SLEEP;
    this->level=0;
    this->name=to_string(nodeId);
    this->parent=NULL;
    this->bestWeight=INT_MAX;
    this->bestNode=NULL;
    this->rec=-1;
    this->testNode=NULL;
    this->nodeId=nodeId;
    //cout<<"Constructor End : "<<nodeId<<endl;
  }

  //--------------------Helper Functions----------------------
  int getIndex(int weight)
  {
    //cout<<"getIndex Start:"<<this->nodeId<<" Weight:"<<weight<<endl;
    int index=-1;
    for(int i=0;i<adjacentEdges.size();i++)
    {
      if(this->adjacentEdges[i]->weight==weight)
      {
        index=i;
        break;
      }
    }
    //cout<<"getIndex End:"<<this->nodeId<<" Weight:"<<weight<<endl;
    return index;

  }

  edge* findEdgeTo(Node* neighbour)
  {
    //cout<<"findEdgeTo Start:"<<this->nodeId<<endl;
    edge* toReturn;
    for(int i=0;i<this->adjacentEdges.size();i++)
    {
      if(this->adjacentEdges[i]->first->nodeId==neighbour->nodeId || this->adjacentEdges[i]->second->nodeId==neighbour->nodeId)
      {
        toReturn=this->adjacentEdges[i];
        break;
      }
    }
    //cout<<"findEdgeTo end:"<<this->nodeId<<endl;
    return toReturn;
  }
  //---------------------Initial Connect Request-----------------------
  void initialConnect()
  {
    //cout<<"Initial Wakeup(initialConnect) start: "<<this->nodeId<<endl;
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

    message* messageToSend=(message*)malloc(sizeof(message));

    messageToSend->message=CONNECT;
    messageToSend->arguments[0]=this->level;
    messageToSend->arguments[1]=getValue.first->weight;


    if(getValue.first->first->nodeId==this->nodeId)
      getValue.first->second->sendMessage(messageToSend);
    else
      getValue.first->first->sendMessage(messageToSend);
    //cout<<"Initial Wakeup(initialConnect) end: "<<this->nodeId<<endl;
  }

  //--------------------Process incoming connect request-------------------
  void processConnectRequest(int level,int weight)
  {
    //cout<<"processConnectRequest start:"<<this->nodeId<<endl;
    int index = getIndex(weight);
    if(index==-1)
      return;

    if(level<this->level)
    {
      this->adjacentEdges[index]->state=BRANCH;
      message* messageToSend=(message*)malloc(sizeof(message));
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
    //cout<<"processConnectRequest end:"<<this->nodeId<<endl;

  }

  //------------------------Process Initiate Message------------------
  void processInitiateRequest(int level,nodeState state,int weight,string name)
  {
    cout<<"processInitiateRequest start:"<<this->nodeId<<endl;
    int index = getIndex(weight);
    if(index==-1)
      return;
    this->level = level;
    this->state = state;
    this->name = name;
    Node* temp;
    cout<<"idhar:"<<this->nodeId<<endl;
    if(adjacentEdges[index]->first->nodeId==this->nodeId)
    {
      cout<<"here1"<<this->nodeId<<endl;
      this->parent = adjacentEdges[index]->second;
      temp = adjacentEdges[index]->second;
    }
    else
    {
      cout<<"here2"<<this->nodeId<<endl;
      this->parent = adjacentEdges[index]->first;
      temp = adjacentEdges[index]->first;
    }

    this->bestNode=NULL;
    this->bestWeight=INT_MAX;
    this->testNode=NULL;



    for(int i=0;i<neighbours.size();i++)
    {
      edge* edgeBetween = findEdgeTo(neighbours[i]);
      if(edgeBetween->state==BRANCH && temp->nodeId!=neighbours[i]->nodeId)
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

    if(state==FIND)
    {
      cout<<"here3"<<this->nodeId<<endl;
      this->rec=0;
      this->testPhase(weight);
    }
    cout<<"processInitiateRequest end:"<<this->nodeId<<endl;
  }

  void testPhase(int weight)
  {
    cout<<"TestPhase start:"<<this->nodeId<<endl;
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
      report(weight);
    }
    cout<<"TestPhase end:"<<this->nodeId<<endl;
  }


  //------------------------Process Test Message------------------
  void rejectEdge(int weight)
  {
    //cout<<"Reject Edge start:"<<this->nodeId<<endl;
    adjacentEdges[getIndex(weight)]->state=REJECTE;
    //cout<<"Reject Edge end:"<<this->nodeId<<endl;
  }

  void processTestRequest(int level,string name,int weight)
  {
    //cout<<"processTestRequest start:"<<this->nodeId<<endl;
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
      messageToSend->arguments[1]=weight;
      messageToSend->name=name;
      this->sendMessage(messageToSend);
    }
    else if(name==this->name)
    {
      if(edgeBetween->state==BASIC)
      {
        adjacentEdges[getIndex(weight)]->state=REJECTE;
        // node1->rejectEdge(weight);
      }
      if(node1->nodeId!=testNode->nodeId)
      {
        message* messageToSend=(message*)malloc(sizeof(message));
        messageToSend->message=REJECT;
        messageToSend->arguments[0]=weight;
        node1->sendMessage(messageToSend);
      }
      else
        testPhase(weight);
    }
    else
    {
      message* messageToSend=(message*)malloc(sizeof(message));
      messageToSend->message=ACCEPT;
      messageToSend->arguments[0]=weight;
      node1->sendMessage(messageToSend);
    }
    //cout<<"processTestRequest end:"<<this->nodeId<<endl;
  }

  //------------------------Process Reject Request------------------
  void processRejectRequest(int weight)
  {
    //cout<<"processRejectRequest start:"<<this->nodeId<<endl;
    int ind = getIndex(weight);
    if(adjacentEdges[ind]->state==BASIC)
      adjacentEdges[ind]->state=REJECTE;

    this->testPhase(weight);
    //cout<<"processRejectRequest end:"<<this->nodeId<<endl;
  }

  //------------------------Process Accept Request------------------
  void processAcceptRequest(int weight)
  {
    //cout<<"processAcceptRequest start:"<<this->nodeId<<endl;
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
    this->report(weight);
    //cout<<"processAcceptRequest end:"<<this->nodeId<<endl;
  }


  //-------------------------Report Function------------------------
  void report(int weight)
  {
    cout<<"report start:"<<this->nodeId<<endl;
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
      message* messageToSend=(message*)malloc(sizeof(message));
      messageToSend->message=REPORT;
      messageToSend->arguments[0]=this->bestWeight;
      messageToSend->arguments[1]=weight;
      this->parent->sendMessage(messageToSend);
    }
    cout<<"report end:"<<this->nodeId<<endl;
  }

  //-------------------------Process Report Request-----------------
  void processReportRequest(int bestWt,int weight)
  {
    //cout<<"processReportRequest start:"<<this->nodeId<<endl;
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
        message* messageToSend=(message*)malloc(sizeof(message));
        messageToSend->message=REPORT;
        messageToSend->arguments[0]=bestWt;
        messageToSend->arguments[1]=weight;
        this->sendMessage(messageToSend);
      }
      else if(bestWt>this->bestWeight)
        this->changeRoot(weight);
      else if(bestWt==this->bestWeight && bestWt==INT_MAX)
      {
        stopFlag=1;
      }
    }
    //cout<<"processReportRequest end:"<<this->nodeId<<endl;
  }

  //------------------------Change Root Function--------------------
  void changeRoot(int weight)
  {
    //cout<<"changeRoot start:"<<this->nodeId<<endl;
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
      messageToSend->arguments[0]=weight;
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
    //cout<<"changeRoot end:"<<this->nodeId<<endl;
  }

  //------------------------Process Change root message-------------
  void processChangeRootRequest(int weight)
  {
    //cout<<"processChangeRootRequest start"<<this->nodeId<<endl;
    changeRoot(weight);
    //cout<<"processChangeRootRequest end"<<this->nodeId<<endl;
  }


  //------------------------sendMessage to a node-------------------
  void sendMessage(message* msg)
  {
    //cout<<"sendMessage start:"<<this->nodeId<<endl;
    this->shareMutex.lock();
    this->messageQueue.push(msg);
    this->shareMutex.unlock();
    //cout<<"sendMessage end:"<<this->nodeId<<endl;
  }

  //------------------------Read message from the top-----------------
  void readMessageFromTop()
  {

    if(!messageQueue.empty())
    {
//cout<<"readMessageFromTop start:"<<this->nodeId<<endl;
      this->shareMutex.lock();
      message* msg = messageQueue.front();
      messageQueue.pop();
      this->shareMutex.unlock();
      //cout<<"Message ID:"<<msg->message<<endl;

      if(this->state==SLEEP)
        this->initialConnect();

      switch(msg->message)
      {
        case START:
          initialConnect();
          break;
        case CONNECT:

          processConnectRequest(msg->arguments[0],msg->arguments[1]);
          ////cout<<"connect"<<endl;
          break;
        case INITIATE:
          processInitiateRequest(msg->arguments[0],msg->state,msg->arguments[1],msg->name);
          ////cout<<"initiate"<<endl;
          break;
        case TEST:
          processTestRequest(msg->arguments[0],msg->name,msg->arguments[1]);
          ////cout<<"test"<<endl;
          break;
        case REJECT:
          processRejectRequest(msg->arguments[0]);
          ////cout<<"reject"<<endl;
          break;
        case ACCEPT:
          processAcceptRequest(msg->arguments[0]);
          ////cout<<"accept"<<endl;
          break;
        case REPORT:
          processReportRequest(msg->arguments[0],msg->arguments[1]);
          ////cout<<"report"<<endl;
          break;
        case CHANGEROOT:
          processChangeRootRequest(msg->arguments[0]);
          ////cout<<"changeroot"<<endl;
          break;
      }
          //cout<<"readMessageFromTop end:"<<this->nodeId<<endl;
    }

  }

  //----------------Find min weight outgoing edge---------------
  pair<edge*,int> findMinEdge()
  {
    //cout<<"findMinEdge start:"<<this->nodeId<<endl;
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
    //cout<<"findMinEdge end:"<<this->nodeId<<endl;
    return retPair;
  }
};


// int main()
// {
//   Node temp(4);
//   temp.processInitiateRequest(4,FIND,5,"hello");
//   pair<edge*,int> retPair;
//   retPair.second=-1;
//   ////cout<<retPair.second<<endl;
//   return 0;
// }
