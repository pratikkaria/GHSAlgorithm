#include<bits/stdc++.h>
//#include "headers.h"
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

void printAdjacentEdges(ofstream *logFile)
{
  for(int j=0;j<allNodes.size();j++)
  {
    *logFile<<"Adjacent Edges Of:"<<allNodes[j]->nodeId<<"\n";
    *logFile<<"First\t"<<"Second\t"<<"Weight\t"<<"State\t"<<"\n";
    for (auto i : allNodes[j]->adjacentEdges)
    {
      int destNodeId = i.first;
      edge* edgeTo = i.second;
      *logFile<<allNodes[j]->nodeId<<"\t"<<destNodeId<<"\t"<<edgeTo->weight<<"\t"<<edgeTo->state<<"\t"<<edgeTo->destNode->nodeId<<"\n";
    }
    // for(int i=0;i<node->adjacentEdges.size();i++)
    //   cout<<myId<<"\t"<<node->adjacentEdges[i]->destNode->nodeId<<"\t"<<node->adjacentEdges[i]->weight<<"\t"<<node->adjacentEdges[i]->state<<endl;
    *logFile<<"---------------------"<<"\n";
  }
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

void postProcessing()
{
  map<int,pair<int,int>> mst;
  for(int j=0;j<mstEdges.size();j++)
  {
    vector<int> temp = processLine(mstEdges[j]);
    for(auto i:allEdges)
      if(i.first==temp[2])
        mst[i.first]=i.second;
  }

  //-----------------Displaying MST----------------
  for(auto i:mst)
    cout<<"("<<i.second.first<<", "<<i.second.second<<", "<<i.first<<")"<<endl;
}

// void logData(double executionTime)
// {
//   ofstream logFile;
//   logFile.open ("logs.txt");
//   logFile<<"Execution Time of the program: "<<executionTime<<" seconds\n";
//   for(int i=0;i<allNodes.size();i++)
//     logFile<<"Number of Messages Sent by Node:"<<i<<" is "<<allNodes[i]->numberOfMessages<<"\n";
//
//
//   logFile.close();
// }


void logSummary(double executionTime)
{
  ofstream logFile;
  logFile.open ("summary.txt");
  printAdjacentEdges(&logFile);

  logFile<<"Execution Time of the program: "<<executionTime<<" seconds\n";
  for(int i=0;i<allNodes.size();i++)
    logFile<<"Number of Messages Sent by Node:"<<i<<" is "<<allNodes[i]->numberOfMessages<<"\n";

  logFile<<"-----------------------\n";
  logFile<<"Final MST: \n";
  logFile<<"-----------------------\n";
  map<int,pair<int,int>> mst;
  for(int j=0;j<mstEdges.size();j++)
  {
    vector<int> temp = processLine(mstEdges[j]);
    for(auto i:allEdges)
      if(i.first==temp[2])
        mst[i.first]=i.second;
  }

  for(auto i:mst)
    logFile<<"("<<i.second.first<<", "<<i.second.second<<", "<<i.first<<")"<<"\n";


  logFile.close();
}
int main(int argc,char** argv)
{
  int all=0;
  clock_t start, end;
  logEnables=0;
  if(argc<2)
  {
    cout<<"Please Enter An Input File Name";
    return 0;
  }
  else if(argc==2)
  {
    if(strcmp(argv[1],"--help")==0)
    {
      cout<<"To Find MST: ./findMST <input_file_name>"<<endl;
      cout<<"To Enable Logs: ./findMST <input_file_name> --enable-logs"<<endl;
      cout<<"To Enable Summary: ./findMST <input_file_name> --enable-summary"<<endl;
      cout<<"To Enable Both: ./findMST <input_file_name> --enable-all-logs"<<endl;
      return 0;
    }
    else if(strcmp(argv[1],"--enable-logs")==0 || strcmp(argv[1],"--enable-summary")==0 || strcmp(argv[1],"--enable-all-logs")==0)
    {
      cout<<"Invalid Option"<<endl;
      cout<<"To Find MST: ./findMST <input_file_name>"<<endl;
      cout<<"To Enable Logs: ./findMST <input_file_name> --enable-logs"<<endl;
      cout<<"To Enable Summary: ./findMST <input_file_name> --enable-summary"<<endl;
      cout<<"To Enable Both: ./findMST <input_file_name> --enable-all-logs"<<endl;
      return 0;
    }
  }
  else if(argc==3)
  {
    if(strcmp(argv[2],"--enable-logs")==0)
      logEnables=2;
    else if(strcmp(argv[2],"--enable-summary")==0)
      logEnables=1;
    else if(strcmp(argv[2],"--enable-all-logs")==0)
    {
        logEnables=2;
        all=1;
    }
    else
    {
      cout<<"Invalid Option. Please Enter Correctly"<<endl;
      return 0;
    }
  }


  start = clock();
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
        allNodes[edgeDetail[0]]->adjacentEdges[edgeDetail[1]]=temp;
        allNodes[edgeDetail[1]]->adjacentEdges[edgeDetail[0]]=temp1;
        allEdges[edgeDetail[2]]=make_pair(edgeDetail[0],edgeDetail[1]);

      }


      stopFlag=0;
      if(logEnables==2)
        logFile.open ("logs.txt");
      thread t[numNodes];
      for(int i=0;i<numNodes;i++)
        t[i]=thread(entryFunction,allNodes[i]);

      allNodes[0]->initialConnect();
      for(int i=0;i<numNodes;i++)
        t[i].join();

      if(logEnables==2)
        logFile.close();
      postProcessing();
      end = clock();
      double time_taken = double(end - start) / double(CLOCKS_PER_SEC);

      if(logEnables==1 || all==1)
        logSummary(time_taken);
      file.close();
  }
  else
  {
    cout<<"File Doesn't Exists"<<endl;
  }
  return 0;
}
