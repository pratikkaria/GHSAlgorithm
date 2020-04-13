#include<thread>
#include<bits/stdc++.h>
using namespace std;

/*
Different types of thread :

Note:
If we create multiple threads at the same time, we are not sure which will execute first

1) Function pointer (Basic)
*/


// void fun(int x)
// {
//   while(x-->0)
//     cout<<x<<endl;
// }
//
// int main()
// {
//   thread t1(fun,11);
//   thread t2(fun,10);
//   t1.join();
//   t2.join();
//   return 0;
// }


//2) Lambda Expression

// int main()
// {
//   //Lambda Function. You can directly inject lambda at thread creation time
//   auto fun=[](int x){
//     while(x-->0)
//       cout<<x<<endl;
//   };
//
//   thread t(fun,10);
//   t.join();
//   return 0;
// }

//or
//
// int main()
// {
//   thread t([](int x){
//     while(x-->0)
//       cout<<x<<endl;
//   },10);
//   t.join();
//   return 0;
// }

//3) Functors
//
// class Base{
// public:
//   void operator ()(int x){
//     while(x-->0)
//       cout<<x<<endl;
//   }
// };
//
// int main()
// {
//   thread t((Base()),10);
//
//   t.join();
//   return 0;
// }


//4) Non static member function

// class Base{
// public:
//   void run(int x){
//     while(x-->0)
//       cout<<x<<endl;
//   }
// };
//
// int main()
// {
//   Base b;
//   thread t(&Base::run,&b,10);
//   t.join();
//   return 0;
// }


//Static member functions
class Base{
public:
  static void run(int x)
  {
    while(x-->0)
      cout<<x<<endl;
  }
};

int main()
{
  thread t(&Base::run,10); //As static so you do not need the object
  t.join();
  return 0;
}
