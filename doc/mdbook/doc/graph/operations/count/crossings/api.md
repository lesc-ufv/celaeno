# Path balancing

<!-- toc -->

## Constraints

```cpp
template<typename T>
concept Iterable = requires{ std::input_iterator<T> && std::incrementable<T>; };

template<typename T>
concept SignedIntegral = std::signed_integral<T>;

template<typename T>
concept Neighbors = requires(T t){ {t(int64_t{})} -> Iterable; };

template<typename T>
concept Edge = requires(T t){ {t(std::pair<int64_t,int64_t>{})} -> std::same_as<std::void_t<>>; };
```

## Interface

```cpp
template<SignedIntegral T, Neighbors P, Neighbors S, Edge L, Edge U>
void run(T root, P&& pred, S&& succ, L&& link, U&& unlink )
```

<!-- vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et : -->
