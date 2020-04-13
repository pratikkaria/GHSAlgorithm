/*
Future and Promise(If you want a value of thread in main)

1) std::promise - used to set values or exceptions
2) std::future - used to get values from promise, ask promise if value is available and wait for promise
*/

#include<bits/stdc++.h>

using namespace std;

typedef long int ull;

void findOdd(promise<ull>&& OddSumPromise,ull start,ull end)
{
  ull oddSum=0;
  for(ull i=start;i<=end;i++)
  {
    if(i&1)
      oddSum+=i;
  }

  OddSumPromise.set_value(oddSum);

}

int main()
{
  ull start=0,end=1900000000;
  promise<ull> oddSum;
  future<ull> oddFuture=oddSum.get_future();

  cout<<"Thread Created:"<<endl;

  thread t1(findOdd,move(oddSum),start,end);
  cout<<"Waiting for Result"<<endl;

  cout<<"Odd Sum: "<<oddFuture.get()<<endl; //Waits till promise sets the value. Promise and future share states

  cout<<"Completed"<<endl;

  t1.join();
  return 0;
}
