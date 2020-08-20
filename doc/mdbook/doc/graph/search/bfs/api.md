# Breadth-First Search

<!-- toc -->

## Constraints
```cpp
template<typename T>
concept SignedIntegral = std::signed_integral<T>;

template<typename T>
concept Iterable = requires{ std::input_iterator<T> && std::incrementable<T>; };

template<typename T>
concept Neighbors = requires(T t){ {t(int64_t{})} -> Iterable; };

template<typename T>
concept Callback = requires(T t){ {t(int64_t{})} -> std::same_as<bool>; };
```

## Interface

```cpp
template<SignedIntegral T, Neighbors P, Neighbors S, Callback C = std::function<bool(int64_t)>>
std::vector<T> run(T root, P&& pred, S&& succ, C&& cb = [](auto&&){return false;});
```

<!-- vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et : -->
