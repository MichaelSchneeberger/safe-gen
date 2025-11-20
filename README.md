# Safe C++ Generator

*SafeGen* is a safe wrapper around *std::generator* that ensures captured values remain valid through its lifetime.
This allows for *Python*-like generator formulations.

## Motivation

Using *std::generator* with lambdas that capture local variables is unsafe. The lifetime of the captured variables is tied to the closure, not the generator itself. If the closure is destroyed while the generator is still alive, iteration can access dangling references.

*SafeGen* solves this problem by:

* Storing both the generator-producing closure and the generator instance together in a single owning object.
* Enforcing that the generator can only be moved or iterated in a safe way.

## Installation


The simplest way to use *safegen* in another project is via FetchContent. See `CMakeLists.txt` in `examples/basic` as an example.


## Basic Example

The example below demonstrates how to create a *SafeGen* from a move-only generator `source` and safely iterate over its elements:

<!-- * How to create a *SafeGen* from a move-only iterable `source`.
* How to consume the first element of a *SafeGen* using the `safegen::next` function,
and then iterate over the remaining elements safely. -->

``` cpp
#include <ranges>
#include <generator>
#include <print>

import safegen;

template<std::ranges::input_range R>
SafeGen<int> add_one(R&& source) {
  // Create a SafeGen from a move-only input range
  return {[source = std::move(source)]() mutable -> std::generator<int> {
    for (int i : source) {
      co_yield i + 1;
    }
  }};
}


int main() {
  // Create a SafeGen that yields 0, 1, 2
  auto source = []() -> std::generator<int> {
    co_yield 0;
    co_yield 1;
    co_yield 2;
  }();

  // Create a new SafeGen that yields 1, 2, 3
  auto one_added = add_one(std::move(source));

  for (auto i : one_added) {
    std::println("{}", i);
  }}
```

The code outputs:

```
1
2
3
```

### Unsafe Alternative

Implementing `add_one` directly with a generator can cause runtime errors due to dangling captures:

``` cpp
template<std::ranges::input_range R>
std::generator<int> add_one(R&& source) {
  return [source = std::move(source)]() mutable -> std::generator<int> {
    for (int i : source) {
      co_yield i + 1;
    }
  }();
}


int main() {
  auto source = []() -> std::generator<int> {
    co_yield 0;
    co_yield 1;
    co_yield 2;
  }();

  auto one_added = add_one(std::move(source));

  // Runtime error occurs here
  for (auto i : one_added) {
    std::println("{}", i);
  }}

```

This produces an assertion failure because the underlying closure may be destroyed while the generator is still active.


## Operations

*SafeGen* provides several operations for safe and expressive generator handling:

- `next` - Safely consumes the first element of a generator and returns it along with a *SafeGen* containing the remaining elements (see `examples/basic/main.cpp`).
- `pairwise` - Transforms a generator of values into a generator of consecutive pairs.

Both operations `next` and `pairwise` are not safely realizable when directly using generators.
