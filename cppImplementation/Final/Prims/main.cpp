#include<bits/stdc++.h>
using namespace std;

pair<int,int> pairOfNodeWeight;
map<int,vector<pair<int,int>>> adjacencyList;
map<int,pair<int,int>> mst;

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

void findMST(int src,int numNodes)
{
  priority_queue<int, vector<int>, greater<int>> minHeap;
  int weightAtEachNode[numNodes];
  int parent[numNodes];
  bool flag[numNodes];
  for(int i=0;i<numNodes;i++)
  {
    weightAtEachNode[i]=INT_MAX;
    parent[i]=-1;
    flag[i]=false;
  }

  minHeap.push(src);
  weightAtEachNode[src]=0;

  while(!minHeap.empty())
  {
    int temp = minHeap.top();
    minHeap.pop();
    flag[temp] = true;
    for(int i=0;i<adjacencyList[temp].size();i++)
    {
      int destNode = adjacencyList[temp][i].first;
      int weight = adjacencyList[temp][i].second;

      if(flag[destNode]==false && weightAtEachNode[destNode]>weight)
      {
        weightAtEachNode[destNode]=weight;
        minHeap.push(destNode);
        parent[destNode]=temp;
        mst[weight]=make_pair(parent[destNode],destNode);
      }
    }
  }


  for(auto i:mst)
    cout<<"("<<i.second.first<<", "<<i.second.second<<", "<<i.first<<")"<<endl;
}

int main(int argc,char** argv)
{
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
          //
          // for(int i=0;i<numNodes;i++)
          // {
          //
          //   Node* tempPtr =new Node(i);
          //   allNodes.push_back(tempPtr);
          // }
          continue;
        }

        vector<int> edgeDetail=processLine(line);
        adjacencyList[edgeDetail[0]].push_back(make_pair(edgeDetail[1],edgeDetail[2]));
        adjacencyList[edgeDetail[1]].push_back(make_pair(edgeDetail[0],edgeDetail[2]));
      }


      findMST(0,numNodes);

      file.close();
  }
}
