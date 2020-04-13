/*
Recursive Mutex

1) It is same as mutes, but same thread can lock one mutex multiple times using recursive_mutex
2) If thread t1 calls lock/try_lock on recursive mutex m1, m1 is locked by t1. Now as t1 is running
in recursion t1 can lock/try_lock any number of times there is no issue.
3) But if t1 has acquired lock 10 times then it has to unlock it 10 times otherwise no other thread
will be able to lock it
4) It is not defined how many times we can lock a recursive mutex but when that number reaches
we get a system error and for try_lock we get false s
*/

#include<bits/stdc++.h>
using namespace std;

recursive_mutex m1;
int buffer = 0;

//Example 1: With recursion

// void recursion(char c,int loopFor)
// {
//   if(loopFor<0)
//     return ;
//
//   m1.lock();
//   cout<<c<<" "<<buffer++<<endl;
//   recursion(c,--loopFor);
//   m1.unlock();
// }
//
// int main()
// {
//   thread t1(recursion,'0',10);
//   thread t2(recursion,'1',10);
//
//   t1.join();
//   t2.join();
//   return 0;
// }

//Example 2: With loop
// recursive_mutex m1;
int main()
{
  for(int i=0;i<5;i++)
  {
    m1.lock();
    cout<<"Locked "<<i<<endl;
  }

  for(int i=0;i<5;i++)
  {
    m1.unlock();
    cout<<"Unlocked "<<i<<endl;
  }
  return 0;
}
