#include<bits/stdc++.h>

using namespace std;

enum messageType
{START,CONNECT,INITIATE,TEST,REJECT,ACCEPT,REPORT,CHANGEROOT};

enum edgeState
{BASIC,BRANCH,REJECTE};

enum nodeState
{SLEEP,FIND,FOUND};

class Node;

typedef struct edge
{
  Node* destNode;
  int weight;
  edgeState state;
}edge;

typedef struct message
{
  messageType message;
  string name;
  nodeState state;
  int arguments[2];
}message;

map<int,pair<int,int>> allEdges;
vector<string> mstEdges;
mutex addEdgeToMSTLock;
int stopFlag;
