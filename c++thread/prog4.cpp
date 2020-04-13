/*
Mutex(Mutual Exclusion in C++)

Race Condition
1) Situation where 2 or more threads/processes happpened to change a common data at the same time.
2) If there is a race condition they we have to protect it and protected section is called critical section

Mutex
1)Used to avoid race Condition
2) lock() and unlock() on mutex to avoid race condition

*/

#include<thread>
#include<bits/stdc++.h>
using namespace std;

int myAmount=0;
mutex m;
void addMoney()
{
  m.lock();
  ++myAmount;
  m.unlock();
}
int main()
{
  thread t1(addMoney);
  thread t2(addMoney);
  t1.join();
  t2.join();
  cout<<myAmount<<endl;
  return 0;
}
