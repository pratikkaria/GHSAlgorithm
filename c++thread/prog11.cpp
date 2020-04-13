/*
Condition Variable

Uses:
1) Notify other threads
2) Waiting for some conditions

1)Condition variable allows running thread to wait on some conditions and once those conditions are met
the waiting thread are notified using:
  - notify_one()
  - notify_all()
2)You need mutex to use condition Variable
3)If thread has to wait on some condition, it has to do 3 things:
  - acquire mutexlock using unique_lock<mutex> lock(m1)
  - execute wait,wait_for or wait_until. The wait operation automatically release the mutex and suspends
  execution of the thread
  - When condition variable is notified, the thread is awakened, and the mutex is automatically acquired.
  The thread should then check the condition and resume waiting if wake up was spurious

Note:
- CV are used to synchronize 2 or more  threads
- Best use case of CV is producer-consumer problem
*/

#include<bits/stdc++.h>
using namespace std;

condition_variable cv;
mutex m;
long balance=0;
void addMoney(int money)
{
  lock_guard<mutex> lg(m);
  balance+=money;
  cout<<"Amount added to current balance:"<<balance<<endl;
  cv.notify_one(); //Notify all the threads waiting for this to complete
}

void withdrawMoney(int money)
{
  unique_lock<mutex> ul(m);
  cv.wait(ul,[]{
    return balance!=0?true:false;
  });

  if(balance>=money)
  {
    balance-=money;
    cout<<"Amount deducted:"<<money<<endl;
  }
  else
    cout<<"Amount can't be deducted. Balance is less than "<<money<<endl;

  cout<<"Current Balance:"<<balance<<endl;
}
int main()
{
  // thread t1(withdrawMoney,500);
  thread t1(withdrawMoney,600);
  thread t2(addMoney,500);
  t1.join();
  t2.join();
  return 0;
}
