#include<bits/stdc++.h>
// #include "headers.h"
#include "createMST.cpp"
using namespace std;

//---------------Function for thread to execute---------------
void startThreads(Node* node)
{
  cout<<node->nodeId<<endl;
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
  return out;
}

int main(int argc,char** argv)
{
  edge* temp = (edge*)malloc(sizeof(edge));
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

            Node* tempPtr =new Node(i+1);
            allNodes.push_back(tempPtr);
          }
          continue;
        }

        vector<int> edgeDetail=processLine(line);
        temp->first=allNodes[edgeDetail[0]];
        temp->second=allNodes[edgeDetail[1]];
        temp->weight=edgeDetail[2];
        temp->state=BASIC;
        allNodes[temp->first]->adjacentEdges.push_back(temp);
        allNodes[temp->second]->adjacentEdges.push_back(temp);
        allNodes[temp->first]->neighbours.push_back(allNodes[temp->second]);
        allNodes[temp->second]->neighbours.push_back(allNodes[temp->first]);
        allEdges.push_back(temp);
      }
      for(int i=0;i<numNodes;i++)
      {
        startThreads(allNodes[i]);
      }
      cout<<"Number of nodes:"<<numNodes<<endl;
      cout<<allEdges.size()<<endl;
      file.close();
  }

  return 0;
}
