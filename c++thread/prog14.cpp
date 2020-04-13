/*
std::async -> Return value from function
*/
#include<bits/stdc++.h>
using namespace std;

typedef long int ull;

ull findOdd(ull start,ull end)
{
  ull oddSum=0;
  for(ull i=start;i<=end;i++)
    if(i&1)
      oddSum+=i;

  return oddSum;
}

int main()
{
  ull start=0,end=1900000000;
  cout<<"Thread created if policy is std::launch::async"<<endl;
  // future<ull> oddSum=async(launch::deferred,findOdd,start,end); //Will not create a Thread
  future<ull> oddSum=async(launch::async,findOdd,start,end); //Starts immediately
  cout<<"Waiting for result"<<endl;

  cout<<"Odd Sum:"<<oddSum.get()<<endl; //launch::deferred creates a thread here and not at async function
  cout<<"Completed"<<endl;
  return 0;
}
