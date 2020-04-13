/*
1) std::try_lock(m1,m2..) tries to lock all lockable objects passed in it one by one in a given order
2) On success it returns -1 else it returns o-based mutex index number which it could not lock.
3) If it fails to lock any mutex, then it releases all mutex it locked till there
4) If call for try_lock raises an exception, unlock is called on any locked objects before throwing
*/

mutex m1,m2;
int X=0,Y=0;
int main()
{
  thread t1(incrementXY,ref(X),ref(m1),"X");
  thread t2(incrementXY,ref(Y),ref(m2),"Y");
  thread t3(consumeXY);
  t1.join();
  t2.join();
  t3.join();
  return 0;
}
