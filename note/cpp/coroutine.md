# Coroutines
> [Daily Bite of C++ Coroutines: Step by Step](https://simontoth.substack.com/p/daily-bite-of-c-coroutines-step-by)
C++20标准库中新增了对协程的支持。对于大多数C++用户来说，使用协程只需要遵循库的文档，提供协程类型即可。但是，如果你需要实现自定义协程类型，本篇文章就是为你准备的。
在本篇文章中，我们将介绍七种不同的协程类型，每个类型都介绍了更多的概念。

## 使用协程 
满足以下两个条件的任何函数（除了`main`）都可以成为一个协程：
- 函数体包含至少一个协程关键字：co_return, co_yield, co_await
- 返回类型符合协程要求（直接或通过std::coroutine_traits）
编译器会为每个协程生成所需的额外代码以处理细节。
标准库提供了`std::generator`（C++23），可以用来实现惰性计算元素序列。`std::generator`可以通过`co_yield`关键字来生成元素。
- co_yield value : 生成单个元素
- co_yield std::ranges::elements_of(range) : 生成一个满足`range`的元素序列
下例子展示了如何使用std::generator来实现斐波那契数列。
```cpp
#include <generator>
#include <print>
#include <utility>

// The return type has to conform to coroutine requirements
std::generator<int> fibonacci(int cnt) {
    int a = 0;
    int b = 1;
    for (;cnt > 0; --cnt)
        // and the function must use at least one coroutine keyword
        co_yield std::exchange(a, std::exchange(b, a+b));
    // This coroutine naturally ends after the loop is exhausted
    // but we can also co_return to exit immediately.
}

int main() {
    for (int v : fibonacci(10)) {
        std::println("{}", v);
    }
}
```
`std::generator` 提供了一个`range`的接口，这意味着从调用者的角度来看，这只是对一个函数的调用，该函数产生一个可迭代的`range`。`std::generator`对于那些依赖于回调函数或者需要提前计算输出的非协程实现非常有用。例如，使用`std::generator`实现不同树的遍历结果就非常简单。
```cpp
#include <print>
#include <generator>

struct Node {
    int value;
    Node* left;
    Node* right;
};

std::generator<Node*> pre_order(Node* root) {
    if (root == nullptr) co_return;

    co_yield root;
    co_yield std::ranges::elements_of(pre_order(root->left));
    co_yield std::ranges::elements_of(pre_order(root->right));
}

std::generator<Node*> in_order(Node* root) {
    if (root == nullptr) co_return;

    co_yield std::ranges::elements_of(in_order(root->left));
    co_yield root;
    co_yield std::ranges::elements_of(in_order(root->right));
}

std::generator<Node*> post_order(Node* root) {
    if (root == nullptr) co_return;

    co_yield std::ranges::elements_of(post_order(root->left));
    co_yield std::ranges::elements_of(post_order(root->right));
    co_yield root;
}


#include <vector>
#include <memory>

std::vector<std::unique_ptr<Node>> make_tree();

int main() {
    auto tree = make_tree();
    Node* root = tree[0].get();

    for (Node* node : pre_order(root))
        std::print("{} ", node->value);
    std::println("");

    for (Node* node : in_order(root))
        std::print("{} ", node->value);
    std::println("");

    for (Node* node : post_order(root))
        std::print("{} ", node->value);
    std::println("");
}

std::vector<std::unique_ptr<Node>> make_tree() {
    std::vector<std::unique_ptr<Node>> tree;
    
    tree.push_back(std::make_unique<Node>(4));
    Node* root = tree.back().get();
    
    tree.push_back(std::make_unique<Node>(2));
    Node* left = tree.back().get();
    root->left = left;

    tree.push_back(std::make_unique<Node>(6));
    Node* right = tree.back().get();
    root->right = right;

    tree.push_back(std::make_unique<Node>(1));
    left->left = tree.back().get();
    tree.push_back(std::make_unique<Node>(3));
    left->right = tree.back().get();

    tree.push_back(std::make_unique<Node>(5));
    right->left = tree.back().get();
    tree.push_back(std::make_unique<Node>(7));
    right->right = tree.back().get();

    return tree;
}
```
通过其他库提供的协程类型也许会提供不同的接口和行为。因此，在使用自定义协程类型之前，请始终检查相应的文档。

## 创建协程
在使用C++的coroutine之前，要明确一件事情：C++的coroutine并不是一个预定义的异步模型。相反，coroutine类型可以适配几乎任何工作流。
创建一个协程需要明确三个主要部分：返回类型（例如std::generator），promise类型（高层描述协程行为）和awaitable类型（控制低层机制如何暂停和恢复协程）。
在下面的章节中，我们将介绍七种不同的协程类型，每个类型都介绍了更多的概念。
### 创建一个例程
coroutine是一般例程的泛化。因此，让我们从实现一个返回void的函数类型开始来逐步实现一个coroutine类型。。
通过一个简单的routine类型，我们可以快速地了解coroutine的promise_type中所有可用的自定义点。
```cpp
#include <coroutine>
#include <print>

struct RoutinePromise;

struct Routine {
    // The return type has to contain a promise_type
    using promise_type = RoutinePromise;
};

struct RoutinePromise {
    // This function is used to create the instance
    // of the return type for the caller
    Routine get_return_object() { return {}; }

    // What should happen before the coroutine body starts
    std::suspend_never initial_suspend() noexcept { return {}; }
    // What should happen after the coroutine body has finished
    std::suspend_never final_suspend() noexcept { return {}; }
    // What should happen when the coroutine executes co_return;
    void return_void() {}
    // What should happen when there is an unhandled exception
    void unhandled_exception() {}
};

int main() {
    auto coro = [] -> Routine {
        std::println("Running...");
        co_return;
    };

    auto x = coro(); // coroutine starts and runs to completion
    // decltype(x) == Routine
    static_assert(std::is_same_v<decltype(x), Routine>);

    coro(); // Because the return type is empty, this is the same as above
}
```
`promise_type`可以定义为一个嵌套类型，而不必像上面的例子中通过一个using别名来定义。
首先介绍`promise_type`中的两个自定义点：`initial_suspend`和`final_suspend`。这两个函数返回一个awaitable类型（我们将在下一节讨论）。`initial_suspend`在协程体开始之前被调用，`final_suspend`在协程体结束之后被调用。
通过awaitable类型，我们可以来控制下面三个行为：
- 继续运行
- 返回控制给调用者(或最后一个resumer)
- 转移控制给另一个协程
C++标准提供了两种awaitable类型：std::suspend_never（协程继续运行）和std::suspend_always（控制返回给调用者）。
在一般的例程中，我们希望协程一直运行到完成，因此，我们使用std::suspend_never作为initial_suspend和final_suspend的返回类型。
```cpp
auto coro = [] -> Routine {
    std::println("Running...");
    co_return;
};

auto x = coro(); // coroutine starts and runs to completion

coro(); // Because the return type is empty, this is the same as above
```
上述实现的一个问题在于，它对返回类型本身是强制的(对应上述例子中的`Routine`)。由于Routine是我们自己实现的类型，我们可以为它指定一个`promise_type`，但当我们希望能够编写一个返回类型来自一个你无法修改的第三方库的协程，上面的实现是不合适的。对于这种情况，标准库提供了`std::coroutine_traits`自定义点。
```cpp
#include <coroutine>
#include <print>

// No need to modify
struct Routine {};

struct RoutinePromise {
    // This function is used to create the instance
    // of the return type for the caller
    Routine get_return_object() { return {}; }

    // What should happen before the coroutine body starts
    std::suspend_never initial_suspend() noexcept { return {}; }
    // What should happen after the coroutine body has finished
    std::suspend_never final_suspend() noexcept { return {}; }
    // What should happen when the coroutine executes co_return;
    void return_void() {}
    // What should happen when there is an unhandled exception
    void unhandled_exception() {}
};

// Any coroutine returning Routine
template <typename... Arg>
struct std::coroutine_traits<Routine, Arg...> {
    using promise_type = RoutinePromise;
};

int main() {
    auto coro = [] -> Routine {
        std::println("coro running...");
        co_return;
    };

    auto x = coro(); // coroutine starts and runs to completion
    // decltype(x) == Routine
    static_assert(std::is_same_v<decltype(x), Routine>);

    coro(); // Because the return type is empty, this is the same as above
}
```
如上所示，假如`Routine`类型来自一个第三方库，我们可以在`std::coroutine_traits`中添加一个特化，以便于编译器能够找到正确的promise_type。
### 创建一个函数
对于一个例程，我们可以忽略它的返回类型，但当我们实现一个函数时，我们需要一个机制来让协程和调用者（只有对返回类型的实例的访问权）进行通信。
C++标准库提供了`std::coroutine_handle`类型来控制对应的协程和访问它的`promise`类型。为了使下面的示例简洁，我们将包装`std::coroutine_handle`（具有引用语义）到一个RAII对象中。
```cpp
template <typename promise_type>
struct owning_handle {
    owning_handle() : handle_() {}
    owning_handle(std::nullptr_t) : handle_(nullptr) {}
    owning_handle(std::coroutine_handle<promise_type> handle) : handle_(std::move(handle)) {}

    owning_handle(const owning_handle<promise_type>&) = delete;
    owning_handle(owning_handle<promise_type>&& other) : handle_(std::exchange(other.handle_, nullptr)) {}
    owning_handle<promise_type>& operator=(const owning_handle<promise_type>&) = delete;
    owning_handle<promise_type>& operator=(owning_handle<promise_type>&& other) {
        handle_ = std::exchange(other.handle_, nullptr);
        return *this;
    }
    promise_type& promise() { return handle_.promise(); }
    ~owning_handle() {
        if (handle_)
            handle_.destroy();
    }
private:
    std::coroutine_handle<promise_type> handle_;
}
```
注意我们在析构函数中显式销毁了协程。
向调用者传递返回值的一个简单的解决方法是将结果存储在promise中。如果我们给调用者访问协程句柄的权限，调用者可以通过句柄读取存储的值。
但是，当coroutine运行结束，promise被销毁，因此我们需要保持协程的存活。
```cpp
#include <coroutine>
#include <print>
#include <utility>

template <typename T>
struct Function;

template <typename T>
struct FunctionPromise {
    using handle_t = std::coroutine_handle<FunctionPromise<T>>;
    // To read the result from the coroutine, 
    // the caller needs to have access to it
    Function<T> get_return_object() {
        // Construct a coroutine handle from this promise
        return Function<T>{handle_t::from_promise(*this)};
    }

    std::suspend_never initial_suspend() noexcept { return {}; }
    // Because we need to give the caller a chance to read 
    // the result, the coroutine now cannot finish after
    // completing the body. It needs to stick around.
    std::suspend_always final_suspend() noexcept { return {}; }

    // The mechanism used to "return" a result.
    // We can simply store it.
    template <std::convertible_to<T> Arg>
    void return_value(Arg&& result) {
        result_ = std::forward<Arg>(result);
    }

    void unhandled_exception() {}

    // The result of the coroutine stored in the promise
    T result_;
};

template <typename T>
struct Function {
    using promise_type = FunctionPromise<T>;

    // Store the coroutine handle
    explicit Function(promise_type::handle_t handle)
      : handle_(handle) {}

    // Interface for extracting the result
    T get() { return handle_.promise().result_; }
    operator T() { return get(); }
private:
    owning_handle<promise_type> handle_;
};

int main() {
    auto coro = [] -> Function<int> {
        std::println("Running...");
        co_return 42;
    };

    auto x = coro();
    int result = x.get(); // Get the result
    std::println("result == {}", result);

    int y = coro(); // Same, but through implicit conversion
    std::println("y == {}", y);
}
```
在`get_return_object`中，我们将对协程句柄的访问权限传递给返回对象,从而使调用者可以访问协程的结果。在`final_suspend`中，我们返回`std::suspend_always`，这将返回控制给调用者（从而保留协程存活）。
`return_value`方法为协程提供了一个返回值的机制。在本例中，我们允许任何可以转换为T的参数；然而，这完全取决于实现者。你也可以通过提供多个return_value的实现来依赖重载解析。
在我们的示例中，我们需要实现对应的构造函数和一个helper方法来访问协程的结果。
```cpp
template <typename T>
struct Function {
    using promise_type = FunctionPromise<T>;

    // Store the coroutine handle
    explicit Function(promise_type::handle_t handle)
      : handle_(handle) {}

    // Interface for extracting the result
    T get() { return handle_.promise().result_; }
    operator T() { return get(); }
private:
    owning_handle<promise_type> handle_;
};
```
协程返回后依然存活(`final_suspend`),因此清理资源的责任属于返回类型，上面的例子通过一个RAII类型`owning_handle`解决了这个问题。

Click to open in Compiler Explorer.
Handling exceptions
Having implemented a function coroutine type, it’s time to take a detour and discuss handling exceptions. When an exception is not caught within the coroutine, the compiler-generated code will be called the unhandled_exception method.

Our two main options are to call std::terminate or to store the currently in-flight exception and re-emit it once the caller attempts to access the result.


Click to open in Compiler Explorer.
To capture and re-emit an exception, we can use std::current_exception, std::exception_ptr and std::rethrow_exception.


Click to open in Compiler Explorer.
For simplicity, the remaining example will use the std::terminate variant.

Crafting a lazy function
The previous two examples (Routine and Function) were excellent tools for introducing the basics of coroutines. However, we can already write routines and functions even in C.

A lazy function is the first coroutine type that isn’t trivially replicated without coroutines.

The obvious change we need to make is to prevent the coroutine from running until the result is requested. This means changing the result type of initial_suspend to std::suspend_always, which will suspend the coroutine and return the control to the caller.


Click to open in Compiler Explorer.
As a consequence of not starting the coroutine automatically, we move the responsibility for running the coroutine to the result type. We must ensure the coroutine has finished running when we attempt to access the result.


Click to open in Compiler Explorer.
Note that the std::coroutine_handle provides the methods done and resume, which we are routing through our RAII wrapper.


Click to open in Compiler Explorer.
Crafting a generator
While the standard library provides the std::generator, writing our own is still a good exercise and a solid introduction to co_yield.

In previous examples, the coroutine only generated one result. However, we can use co_yield to potentially repeatedly yield values, with the co_return serving only for early termination.

The support for co_yield mirrors the support for co_return with an argument. As in previous examples, we allow any argument that is convertible to T.


Click to open in Compiler Explorer.
The important difference between co_return and co_yield is the awaitable result. This gives us yet another customization point, precisely to control what happens after the coroutine yields a value, with the usual three main options:

the coroutine continues running

the control is returned to the caller/resumer

the control is transferred to another coroutine

In the case of a generator, we want to return the control to the caller (so that the caller can read the generated value), meaning we can return the standard std::suspend_always.


Click to open in Compiler Explorer.
The other side of the coin is the support inside the result type, which now needs to resume the coroutine to generate each value.


Click to open in Compiler Explorer.
While this interface works, it is pretty cumbersome. We have to start the coroutine separately, check if it’s exhausted (which will resume the coroutine), and only then read the current value. The std::generator overcomes this complexity by supporting a range interface, so let’s do the same.

First, we need an iterator type.


Click to open in Compiler Explorer.
To avoid issues with empty coroutines, we have to resume the coroutine when returning the begin iterator. If the coroutine immediately finishes, handle_.done() will return true, meaning that begin == default_sentinel_t{}.


Click to open in Compiler Explorer.
With the range interface in place, we can now use the range for loop to iterate over all generated values.


Click to open in Compiler Explorer.
Cooperative multitasking
So far, we have dealt with a single in-flight coroutine controlled by the caller. The more interesting use cases for coroutines typically involve multiple in-flight coroutines controlled by a scheduler/executor.

We can start with a simple case of cooperative multitasking in which multiple coroutines share the same execution thread and voluntarily give up control of this thread to let other coroutines run.

To achieve that, we will need to discuss awaitable types. We have already seen std::suspend_always (which returns control to the caller) and std::suspend_never (which lets the coroutine continue). We have also discussed three methods that return an awaitable: initial_suspend, final_suspend, and yield_value.

The piece of the puzzle that has been hidden away is the co_await keyword, mainly because this keyword appeared in the code generated by the compiler. The generated calls to the above methods have the form of:

co_await promise().initial_suspend();
co_await promise().final_suspend();
co_await promise().yield_value(arg);
This means that we are invoking co_await on the instance of an awaitable type returned by these methods, which then, in turn, controls what happens next:

the coroutine continues running

the control is returned to the caller/resumer

the control is handed over to another coroutine

Importantly, nothing prevents us from directly invoking co_await in the coroutine body. For example, if we invoke co_await std::suspend_always{}, we will suspend the coroutine and return the control to the caller, who can destroy or resume the coroutine as desired. However, I mentioned a scheduler, so let’s start with that.


Click to open in Compiler Explorer.
This is also the first time we use the type-erased std::coroutine_handle<>. The scheduler only manages the lifetime of coroutines and doesn’t need access to any of the specifics.

For the coroutine type, we return to a simple task with an important addition: a detach method that hands off the coroutine’s ownership to the scheduler.


Click to open in Compiler Explorer.
The final piece is the awaitable type that will suspend a coroutine and let other coroutines run.


Click to open in Compiler Explorer.
When the coroutine evaluates co_await WakeupAwaitable{}, it will be suspended, and the control will be returned to the caller, the scheduler.


Click to open in Compiler Explorer.
Working with asynchrony
When interacting with asynchrony, you will typically need to craft the coroutines to fit your specific needs. However, one type of asynchrony that is reasonably straightforward to integrate with is time. It also introduces an excellent opportunity to talk about await_transform.

The primary change is in our scheduler, which now needs an ordered container for storing the coroutine handles, each of which also has a wakeup time attached.


Click to open in Compiler Explorer.
The natural extension of the interface from the previous example would be co_await Scheduler{}.wake_up(time_point); however, that is a bit cumbersome. It would be a lot cleaner if we could write co_await time_point instead. We can achieve this using the await_transform method on the promise_type.


Click to open in Compiler Explorer.
However, if the promise_type defines at least one await_transform, only types that match one of the await_transform overloads will be usable with co_await.


Click to open in Compiler Explorer.
Awaitable coroutines
The final type of coroutine we will introduce is an awaitable coroutine. In the previous example, we had the coroutine wait for an event.

While event-based triggering is pretty typical in asynchronous systems, sometimes we want to wait for the completion of an operation. It would also be very convenient to model these operations as coroutines.

You might consider simply calling another coroutine directly; however, then you run into two issues:

How do you resume the parent coroutine when the operation finishes (considering that the operation might need to suspend to wait for events or other operations)?

In a system heavily relying on coroutines, we can end up with millions of in-flight coroutines, which can potentially deeply nest (a lot more aggressively than functions). How do we avoid running out of stack space?

These issues can be addressed by adding a layer of indirection through awaitable types that rely on symmetric transfer.


Click to open in Compiler Explorer.
We start with the basic Task interface, which we extend with the awaitable type interface.


Click to open in Compiler Explorer.
When we co_await coroutine(), we want the coroutine to suspend, producing an instance of the result type. The co_await operator will then find the awaitable interface, which, in this case, uses the coroutine handle version of await_supend.

We return false from await_ready to suspend the caller and then return the callee’s handle to start its execution. However, we also remember the caller so that we can resume it once the callee finishes execution.


Click to open in Compiler Explorer.
While this all works, you might wonder whether you can apply the same logic to functions, and in fact, you can. The result of the co_await expression is the result of await_resume.


Click to open in Compiler Explorer.
This allows us to return values from our awaitable coroutines and mix and match tasks with functions.


Click to open in Compiler Explorer.
Conclusion
In this article, we went over seven types of coroutines:

routine

function

lazy function

generator

cooperative tasks

cooperative tasks with asynchrony

awaitable coroutines

While this is already a lot of information, we have still completely ignored many topics. Leave a comment if you would like a follow-up article covering more advanced topics, such as multi-threading with coroutines or custom allocators.

