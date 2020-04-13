/*
Lock Gaurd in c++

lock_gaurd<mutex> lock m1

1) It is lightweight wrapper on owning mutex in scoped basis
2) It acquires mutex object the moment you create the object of lock_gaurd
3) It automatically removes the lock when it goes out of scope
4) You cannot explicitly unlock lock_gaurd
5) You cannot copy lock_gaurd
*/

#include<bits/stdc++.h>
using namespace std;

mutex m1;
int buffer = 0;

void task(const char* threadNumber, int loopFor)
{
  lock_guard<mutex> lock(m1); //Unlocked in lock_gaurd's destructor
  for(int i=0;i<loopFor;i++)
  {
    buffer++;
    cout<<threadNumber<<" "<<buffer<<endl;
  }
}

int main()
{
  thread t1(task,"T0",10);
  thread t2(task,"T1",10);
  t1.join();
  t2.join();
  return 0;
}
