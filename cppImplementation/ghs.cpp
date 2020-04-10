#include<bits/stdc++.h>
// #include "headers.h"
#include "node1.cpp"
#include <unistd.h>
using namespace std;

//---------------Function for thread to execute---------------
void entryFunction(Node* node)
{
  while(!stopFlag)
    node->pullMessage();
// cout<<this_thread::get_id()<<endl;

}

void printAdjacentEdges(int myId,Node* node)
{
  cout<<"Adjacent Edges Of:"<<node->nodeId<<endl;
  cout<<"First\t"<<"Second\t"<<"Weight\t"<<"State\t"<<endl;
  for (auto i : node->adjacentEdges)
  {
    int destNodeId = i.first;
    edge* edgeTo = i.second;
    cout<<myId<<"\t"<<destNodeId<<"\t"<<edgeTo->weight<<"\t"<<edgeTo->state<<"\t"<<edgeTo->destNode->nodeId<<endl;
  }
  // for(int i=0;i<node->adjacentEdges.size();i++)
  //   cout<<myId<<"\t"<<node->adjacentEdges[i]->destNode->nodeId<<"\t"<<node->adjacentEdges[i]->weight<<"\t"<<node->adjacentEdges[i]->state<<endl;
  cout<<"---------------------"<<endl;
}



//---------------Display the vector----------------
template<typename T>
void printVector(vector<T> str)
{
  for(int i=0;i<str.size();i++)
    cout<<str[i]<<endl;
  cout<<"----------------"<<endl;
}

//-------------Function to process each line of input-----------------
vector<int> processLine(string s)
{
  vector<int> out;
  s.erase( std::remove_if(s.begin(), s.end(),[](char ch){ return ch=='(' || ch ==')'; }),s.end());
  s.erase( std::remove_if(s.begin(), s.end(),[](char ch){ return ch==','; }),s.end());
  istringstream ss(s);
  while (ss)
  {
      string word;
      ss >> word;
      if(word=="")
        continue;
      out.push_back(atoi(word.c_str()));
  }
  // printVector(out);
  return out;
}

void printMST()
{
  cout<<"---------------------------------------------------"<<endl;
  for(int i=0;i<mstEdges.size();i++)
    cout<<mstEdges[i]<<endl;
}
int main(int argc,char** argv)
{

  vector<Node*> allNodes;
  //----------------------Reading Input from File--------------------
  ifstream file(argv[1]);
  int numNodes,flag=0;
  if (file.is_open())
  {
      string line;
      //-------------------Creating Edge List and Node List-----------------------
      while (getline(file, line))
      {

        if(flag==0)
        {
          numNodes=atoi(line.c_str());
          flag=1;

          for(int i=0;i<numNodes;i++)
          {

            Node* tempPtr =new Node(i);
            allNodes.push_back(tempPtr);
          }
          continue;
        }

        vector<int> edgeDetail=processLine(line);
        edge* temp = (edge*)malloc(sizeof(edge));
        edge* temp1 = (edge*)malloc(sizeof(edge));
        temp->destNode = allNodes[edgeDetail[1]];
        temp->weight = edgeDetail[2];
        temp->state = BASIC;

        temp1->destNode = allNodes[edgeDetail[0]];
        temp1->weight = edgeDetail[2];
        temp1->state = BASIC;
        // cout<<temp->first->nodeId<<" "<<temp->second->nodeId<<endl;
        allNodes[edgeDetail[0]]->adjacentEdges[edgeDetail[1]]=temp;
        allNodes[edgeDetail[1]]->adjacentEdges[edgeDetail[0]]=temp1;
        // allNodes[edgeDetail[0]]->adjacentEdges.push_back(temp);
        // allNodes[edgeDetail[1]]->adjacentEdges.push_back(temp1);

      }
      for(int i=0;i<allNodes.size();i++)
        printAdjacentEdges(i,allNodes[i]);
      stopFlag=0;
      thread t[numNodes];
      for(int i=0;i<numNodes;i++)
        t[i]=thread(entryFunction,allNodes[i]);

      allNodes[0]->initialConnect();
      for(int i=0;i<numNodes;i++)
        t[i].join();

      printMST();
      cout<<"Number of nodes:"<<numNodes<<endl;
      cout<<allEdges.size()<<endl;
      file.close();
  }

  return 0;
}
