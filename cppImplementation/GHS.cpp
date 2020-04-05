#include<bits/stdc++.h>
#include "headers.h"
// #include "createMST.cpp"
using namespace std;

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
  //-----------Reading Input from File and writing it in allEdges vector-------------
  ifstream file(argv[1]);
  int numNodes,flag=0;
  if (file.is_open())
  {
      string line;
      while (getline(file, line))
      {
        if(flag==0)
        {
          numNodes=atoi(line.c_str());
          flag=1;
          continue;
        }
        vector<int> edgeDetail=processLine(line);
        temp->first=edgeDetail[0];
        temp->second=edgeDetail[1];
        temp->weight=edgeDetail[2];
        temp->state=0;
        allEdges.push_back(temp);
      }
      cout<<"Number of nodes:"<<numNodes<<endl;
      cout<<allEdges.size()<<endl;
      file.close();
  }
  return 0;
}
