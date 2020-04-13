/*
Timed Mutex

1) timed_mutex is blocked till timeout_time or lock is acquired and returns true if success
2) Member function - lock,try_lock,try_lock_for,try_lock_until,unlock

try_lock_for() - waits until specified timeout duration has elapsed or lock is acquired, whichever comes first.
On successful lock acquisition, it returns true else false
*/

#include<bits/stdc++.h>
#include<thread>
using namespace std;

int myAmount=0;
timed_mutex m;
void increment(int i)
{
  /*
  For try_lock_until

  auto now = chrono::steady_clock::now();
  if(m.try_lock_until(now+chrono::seconds(1)))
  {
  ...
}
  */
  if(m.try_lock_for(chrono::seconds(1)))
  {
    ++myAmount;
    this_thread::sleep_for(chrono::seconds(2));
    cout<<"Thread: "<<i<<" Entered"<<endl;
    m.unlock();
  }
  else
  {
    cout<<"Thread "<<i<<" Could not enter"<<endl;
  }
}

int main()
{
  thread t1(increment,1);
  thread t2(increment,2);

  t1.join();
  t2.join();

  cout<<myAmount<<endl;

  return 0;
}
