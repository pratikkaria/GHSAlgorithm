/*
Unique Lock(unique_lock<mutex> lock(m1))

1) It is a mutex ownership wrapper
2) It allows:
  - Can have different locking strategies
  - Time constrained attempts at locking(try_lock_for,try_lock_until)
  - Recursive locking
  - Transfer of lock ownership( move not copy)
  - Condition vars

Locking strategies:
    TYPE                EFFECTS
1) defer_lock           do not acquire ownership of Mutex
2) try_to_lock          try to acquire ownership of mutex without blocking
3) adopt_lock           assume that calling thread already has ownership of mutex.
*/

#include<bits/stdc++.h>
using namespace std;

mutex m1;
int buffer=0;

//Example  1

void task(const char* threadNumber,int loopFor)
{
  // unique_lock<mutex> lock(m1); //Unlocks in destructor
  //or
  unique_lock<mutex> lock(m1,defer_lock); //does not call lock on mutex m1 as defer lock
  lock.lock(); //But we have to explicitly tell it to lock whenever we want to
  for(int i=0;i<loopFor;i++)
  {
    buffer++;
    cout<<threadNumber<<" "<<buffer<<endl;
  }
}

int main()
{
  thread t1(task,"T1",10);
  thread t2(task,"T2",10);
  t1.join();
  t2.join();
  return 0;
}
