# Concept
Concept 是CPP20中新增的概念，用于描述一个类型是否满足某个特定的条件。在C++20之前，我们通常使用模板元编程来实现类似的功能(SFINAE)，但是这种方式比较繁琐，而且容易出错。借助Concept，我们可以更加简洁地实现类似的功能，且报错提示更加简洁。

## SFINAE
SFINAE(Substitution Failure Is Not An Error)是一种模板元编程技术，它允许编译器在模板实例化时检查类型是否满足特定的条件。如果类型不满足条件，编译器会忽略该实例化，从而避免编译错误。
SFINAE的实现方式通常是使用一个特化版本的模板函数，该函数的参数类型为待检查的类型，返回类型为void。如果该函数的实例化成功，则说明类型满足条件，否则不满足条件。
```cpp
#include <iostream>
#include <type_traits>
#include <vector>

// SFINAE测试: 一个辅助类型（helper type）
template<typename T, typename = void>
struct HasSize : std::false_type {};

// 对于有.size()成员函数的类型特化
template<typename T>
struct HasSize<T, std::void_t<decltype(std::declval<T>().size())>> : std::true_type {};

// 仅接受具有.size()成员函数的类型的函数
template<typename T>
typename std::enable_if<HasSize<T>::value, void>::type printSize(const T& container) {
    std::cout << "Size: " << container.size() << std::endl;
}

// 重载的版本，用于那些不具有.size()成员函数的类型
void printSize(...) {
    std::cout << "Type does not have a size." << std::endl;
}

int main() {
    std::vector<int> vec = {1, 2, 3};
    int arr[3] = {1, 2, 3};

    printSize(vec); // 正确: vector有.size()成员函数
    printSize(arr); // 输出: Type does not have a size.
    return 0;
}
```
上述代码中，我们定义了一个辅助类型`HasSize`，它接受一个类型参数`T`和一个可选的类型参数`U`。如果`U`没有被指定，则默认为`void`。`HasSize`是一个模板类，它有一个成员类型`value`，表示类型`T`是否具有`.size()`成员函数。
同时定义了一个`printSize`函数，它接受一个类型为`T`的参数。如果类型`T`具有`.size()`成员函数，则使用`std::enable_if`来启用该重载。如果类型`T`不具有`.size()`成员函数，则使用`...`参数包匹配来启用另一个重载。
> `std::enable_if`是一个在 C++11 中引入的类型triat，它基于一个编译时布尔表达式，能够在满足条件时启用某个模板特化或函数重载。若条件为真（即，表达式为 true），`std::enable_if` 将有一个名为 `type` 的成员定义，其类型是其第二个模板参数（默认为 `void`）。如果条件为假（即，表达式为 false），则 std::enable_if 没有 `type` 成员，导致替换失败，这也是基于 `SFINAE` 原则实现的。

## Concept
有了Concept，我们可以更加简洁地实现类似的功能。改写如上代码，使用Concept来实现：
```cpp
```
### 定义Concept
一个Concept的定义语法如下：
```cpp
template < template-parameter-list >
concept concept-name attr(optional) = constraint-expression;
```
`constraint-expression`可以是对模板参数 `T` 的一系列要求，通常使用标准库中的类型特性`（std::is_integral_v<T>）`等进行检查，或者使用 `requires` 表达式直接描述 T 要支持的操作。
- 简单的类型检查
```cpp
#include <type_traits>

template<typename T>
concept Integral = std::is_integral_v<T>;
```
- 使用requires表达式
```
#include <concepts>

template<typename T>
concept HasSize = requires(T t) {
    { t.size() } -> std::convertible_to<std::size_t>;
};
```
#### requires表达式
requires表达式是C++20引入的语法，用于描述类型T的约束条件。它可以用于类型检查、函数重载、模板参数推导等场景。
requires的语法如下：
```cpp
requires (parameter-list) {
    requirement-seq
}
```
`parameter-list`是可选的，用于定义requires表达式中的参数列表。这些参数没有链接、存储或生命周期，它们仅作为定义`requirement-seq`的注解使用。
`requirement-seq`是一组requirement，每个requirement有如下几种形式之一：
- simple-requirement:
- type-requirement: 
- compound-requirement:
- nested-requirement:

### 使用Concept
Concept可以用于函数模板的参数类型检查，也可以用于类模板的约束。
#### 函数模板

```cpp
#include <concepts>
template<typename T>
requires std::integral<T>
T add(T a, T b) {
    return a + b;
}
```

