//1) In every application there is a default thread main() inside which we create other threads

//2) Thread is a lightweight process. Idea is to achieve parallelism by dividing process into multiple threads

//Ways to create threads in c++11
//1) Function pointers
//2) Lambda functions
//3) Functors
//4) Member Functions
//5) Static member functions

//Requirement: Find adding of all odd and even nos between 1 and 1900000000


#include<iostream>
#include<bits/stdc++.h>
#include<thread>
#include<chrono>
#include<algorithm>

using namespace std;
using namespace std::chrono;

typedef unsigned long long ull;

ull OddSum = 0;
ull EvenSum = 0;


void findEven(ull start,ull end)
{
  for(ull i=start;i<=end;++i)
  {
    if((i&1)==0)
      EvenSum+=i;
  }
}

void findOdd(ull start,ull end)
{
  for(ull i=start;i<=end;++i)
  {
    if((i&1==1))
      OddSum+=i;
  }
}

int main()
{
  ull start=0,end=1900000000;
  auto startTime = high_resolution_clock::now();

  thread t1(findEven,start,end);
  thread t2(findOdd,start,end);

  t1.join();
  t2.join();

  // findEven(start,end);
  // findOdd(start,end);
  auto stopTime = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stopTime-startTime);
  cout<<"Odd Sum: "<<OddSum<<endl;
  cout<<"Even Sum: "<<EvenSum<<endl;
  cout<<"Duration: "<<duration.count()/1000000<<endl;

  return 0;
}
