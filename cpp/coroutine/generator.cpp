#include <coroutine>
#include <iostream>
#include <vector>

template<typename T>
struct Generator {
    struct promise_type {
        T value;
        std::suspend_always yield_value(T v) {
            value = v;
            return {};
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::exit(1); }
        Generator get_return_object() { return Generator{this}; }
    };

    struct iterator {
        std::coroutine_handle<promise_type> coro;
        iterator(std::coroutine_handle<promise_type> h) : coro(h) {}
        bool operator!=(std::nullptr_t) { return !coro.done(); }
        iterator& operator++() { coro.resume(); return *this; }
        T const& operator*() const { return coro.promise().value; }
    };

    std::coroutine_handle<promise_type> coro;
    Generator(promise_type* p) : coro(std::coroutine_handle<promise_type>::from_promise(*p)) {}
    iterator begin() { coro.resume(); return iterator{coro}; }
    iterator end() { return nullptr; }
};

Generator<int> range(int first, int last) {
    for (int i = first; i <= last; ++i) {
        co_yield i;
    }
}

int main() {
    for (int value : range(1, 5)) {
        std::cout << value << " ";
    }
    std::cout << std::endl;
    return 0;
}