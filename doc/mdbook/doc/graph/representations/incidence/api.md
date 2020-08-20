# Incidence Matrix

<!-- toc -->

## Constraints
```cpp
template<typename T>
concept Iterable = requires{ std::input_iterator<T> && std::incrementable<T>; };

template<typename T>
concept Layer =
requires(T t)
{
  { t(i64{})        } -> Iterable;
  { t(i64{})        } -> Iterable;
  { t(i64{}).size() } -> std::integral;
};

template<typename T>
concept Adjacent = requires(T t) {{ t(i64{},i64{}) } -> std::same_as<bool>; };
```

## Interface
```cpp
template<Layer L, Adjacent A>
auto run(L&& get_layer, A&& adjacent, u64 height);
```

<!-- vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et : -->
