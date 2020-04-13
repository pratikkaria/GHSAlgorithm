/*
JOIN Notes
1)Once a thread starts, we wait for the thread to complete using join function on thread object
2)Double join will result in program termination
3)If need check if thread is joinable before joining(Use of joinable())

DETACH Notes
1)This is used to detach newly created thread from its parent thread
2)Always check before detaching a thread that it is joinable otherwise we may end up double detaching
which will result in program termination
3)If we have detached thread and main function is returning then detacheed thread execution is suspended

Notes
1)Either join() or detach() should be called on thread object , otherwise during thread object destructor
it will terminate the program as inside destructor it checks if thread is joinable. If yes then it terminates
the program

The moment you create a thread, the thread becomes joinable. and then when you once apply join or detach
the thread becomes non-joinable
*/

#include<iostream>
#include<chrono>
#include<thread>
#include<bits/stdc++.h>
using namespace std;

void run(int count)
{
  while(count-->0)
    cout<<count<<"pratik"<<endl;

  this_thread::sleep_for(chrono::seconds(3));
}
int main()
{
  thread t1(run,10);
  cout<<"main function"<<endl;
  // t1.join();
  // if(t1.joinable())
  //   t1.join();

  t1.detach(); //it detaches thread from main thread but if main thread ends, this also ends

  cout<<"main after"<<endl;
  return 0;
}
