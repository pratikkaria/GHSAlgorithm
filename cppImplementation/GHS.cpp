#include<bits/stdc++.h>
// #include "headers.h"
#include "createMST.cpp"
using namespace std;

//---------------Function for thread to execute---------------
void entryFunction(Node* node)
{
  while(!stopFlag)
    node->readMessageFromTop();


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
        edge* temp = (edge*)malloc(sizeof(edge));
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
        temp->first=allNodes[edgeDetail[0]];
        temp->second=allNodes[edgeDetail[1]];
        temp->weight=edgeDetail[2];
        temp->state=BASIC;
        // cout<<temp->first->nodeId<<" "<<temp->second->nodeId<<endl;
        allNodes[temp->first->nodeId]->adjacentEdges.push_back(temp);
        allNodes[temp->second->nodeId]->adjacentEdges.push_back(temp);
        allNodes[temp->first->nodeId]->neighbours.push_back(allNodes[temp->second->nodeId]);
        allNodes[temp->second->nodeId]->neighbours.push_back(allNodes[temp->first->nodeId]);
        allEdges.push_back(temp);
      }
      // cout<<allNodes[0]->nodeId<<endl;
      // for(int i=0;i<allNodes[0]->adjacentEdges.size();i++)
      //   cout<<allNodes[0]->adjacentEdges[i]->first->nodeId<<" "<<allNodes[0]->adjacentEdges[i]->second->nodeId<<endl;
      thread t[numNodes];
      for(int i=0;i<numNodes;i++)
        t[i]=thread(entryFunction,allNodes[i]);

      allNodes[0]->initialConnect();
      for(int i=0;i<numNodes;i++)
        t[i].join();
      cout<<"Number of nodes:"<<numNodes<<endl;
      cout<<allEdges.size()<<endl;
      file.close();
  }

  return 0;
}
