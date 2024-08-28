#include <coroutine>
#include <iostream>
struct promise;

struct coroutine : std::coroutine_handle<promise> {
  using promise_type = ::promise;
};

struct promise {
  coroutine get_return_object() { return {coroutine::from_promise(*this)}; }

  std::suspend_always initial_suspend() { return {}; }

  std::suspend_always final_suspend() noexcept { return {}; }

  void return_void() {}

  void unhandled_exception() { std::terminate(); }
};

struct S {
  int i;
  coroutine f() {
    std::cout << i << std::endl;
    co_return;
  }
};

void example() {
  S s = {42};
  coroutine c = s.f();
  std::cout << "before resume\n";
  c.resume();
}
int main() {
  example();
  return 0;
}
