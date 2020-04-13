#include <functional>

std::function<int()> foo;

void *thr(void *)
{
  foo();
}

int main()
{
  pthread_t tid;
  pthread_create(&tid, nullptr, thr, nullptr);
  pthread_join(tid, nullptr);
}
