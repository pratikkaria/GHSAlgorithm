/*
Mutex Try Lock

1)try_lock() tries to lock the mutex. Returns immediately(Does block like lock()). On successful lock acquisition, it Returns
true else false
- If try lock is not able to get the lock then it doesn't get blocked and so it is non blocking
2)If trylock is called by same thread that owns the mutex then it leads to undefined behavior.
It is a deadlock situation with undefined behavior
*/

#include<bits/stdc++.h>
#include<thread>
using namespace std;

int counter=0;
mutex mtx;
void increaseCounterFor100000Time()
{
  for(int i=0;i<100000;i++)
  {
    if(mtx.try_lock())
    {
      ++counter;
      mtx.unlock();
    }
    
  }
}

int main()
{
  thread t1(increaseCounterFor100000Time);
  thread t2(increaseCounterFor100000Time);
  t1.join();
  t2.join();
  cout<<counter<<endl;

  return 0;
}
