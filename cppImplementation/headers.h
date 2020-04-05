#include<bits/stdc++.h>
using namespace std;

enum messageType
{START,CONNECT,INITIATE,TEST,REJECT,ACCEPT,REPORT,CHANGEROOT};

enum edgeState
{BASIC,BRANCH,REJECTE};

enum nodeState
{SLEEP,FIND,FOUND};

typedef struct edge
{
  int first;
  int second;
  int weight;
  edgeState state;
}edge;

typedef struct message
{
  messageType message;
  int arguments[3];
}message;

vector<edge*> allEdges;
vector<edge*> mstEdges;
