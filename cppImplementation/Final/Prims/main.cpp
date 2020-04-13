#include<bits/stdc++.h>
using namespace std;

typedef pair<int,int> pairOfNodeWeight;
map<int,vector<pair<int,int>>> adjacencyList;
map<int,pair<int,int>> mst;
map<int,pair<int,int>> allEdges;
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
  priority_queue<pairOfNodeWeight, vector<pairOfNodeWeight>, greater<pairOfNodeWeight>> minHeap;
  int weightAtEachNode[numNodes];
  int parent[numNodes];
  bool flag[numNodes];
  for(int i=0;i<numNodes;i++)
  {
    weightAtEachNode[i]=INT_MAX;
    parent[i]=-1;
    flag[i]=false;
  }

  minHeap.push(make_pair(0,src));
  weightAtEachNode[src]=0;

  while(!minHeap.empty())
  {
    int temp = minHeap.top().second;
    minHeap.pop();
    flag[temp] = true;
    for(int i=0;i<adjacencyList[temp].size();i++)
    {
      int destNode = adjacencyList[temp][i].first;
      int weight = adjacencyList[temp][i].second;

      if(flag[destNode]==false && weightAtEachNode[destNode]>weight)
      {
        weightAtEachNode[destNode]=weight;
        minHeap.push(make_pair(weightAtEachNode[destNode],destNode));
        parent[destNode]=temp;
        // mst[weight]=make_pair(parent[destNode],destNode);
      }
    }
  }
  // for (int i = 1; i < numNodes; ++i)
  //       printf("%d - %d\n", parent[i], i);

  for(int i=1;i<numNodes;++i)
  {
    int temp1=parent[i];
    int temp2=i;
    for(auto j:allEdges)
    {
      if((j.second.first==temp1 && j.second.second==temp2) || (j.second.second==temp1 && j.second.first==temp2))
      {
        mst[j.first]=make_pair(j.second.first,j.second.second);
        break;
      }
    }
  }
  //
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
        allEdges[edgeDetail[2]]=make_pair(edgeDetail[0],edgeDetail[1]);
        adjacencyList[edgeDetail[0]].push_back(make_pair(edgeDetail[1],edgeDetail[2]));
        adjacencyList[edgeDetail[1]].push_back(make_pair(edgeDetail[0],edgeDetail[2]));
      }


      findMST(0,numNodes);

      file.close();
  }
}
