# Incidence Matrix

<!-- toc -->

## Constraints
```cpp
template<typename T>
concept SignedIntegral = std::signed_integral<T>;

template<typename T>
concept Iterable = requires{ std::input_iterator<T> && std::incrementable<T>; };

template<typename T>
concept Neighbors = requires(T t){ {t(int64_t{})} -> Iterable; };
```

## Interface
```cpp
template<SignedIntegral T, Neighbors N1, Neighbors N2>
auto run(T root, N1&& pred, N2&& succ)
```

<!-- vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et : -->
