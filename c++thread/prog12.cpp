/*
lock() C++

- It is used to lock multiple mutexes at the same time
Syntax:lock(m1,m2,m3,m4);

1) All arguments are locked via a sequence of calls to lock(),try_lock() or unlock() on each argument
2) Order of locking is not defined(it will try to lock the provided mutexes in any order and ensure that
there is not deadlock)
3) It is a blocking call
*/
