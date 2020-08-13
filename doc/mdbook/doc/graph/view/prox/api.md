# Proximity View

<!-- toc -->

## Constraints

```cpp
template<typename T>
concept SignedIntegral = std::signed_integral<T>;

template<typename T>
concept Iterable = requires{ std::input_iterator<T> && std::incrementable<T>; };

template<typename T>
concept Function = requires(T t) { {t(int64_t{})} -> Iterable; };
```

## Interface

```cpp
template<SignedIntegral T, Neighbors P, Neighbors S>
auto run(T root, P&& pred, S&& succ) -> std::pair<std::multimap<T,T>,std::map<T,T>>;
```

<!-- vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et : -->
