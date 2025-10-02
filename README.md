# Safe C++ Generator

*SafeGen* is a safe wrapper around *std::generator* that ensures captured values remain valid through its lifetime.
This allows for *Python*-like generator formulations.

## Motivation

Using *std::generator* with lambdas that capture local variables is unsafe. The lifetime of the captured variables is tied to the closure, not the generator itself. If the closure is destroyed while the generator is still alive, iteration can access dangling references.

*SafeGen* solves this problem by:
- Storing both the generator-producing closure and the generator instance together in a single owning object.
- Enforcing that the generator can only be moved or iterated in a safe way.

## Operations

The following operations are provided for working with *SafeGen*:

- `next` - Safely consumes the first element of a generator and returns it along with a *SafeGen* containing the remaining elements.
- `pairwise` - Transforms a generator of values into a generator of consecutive pairs.

## Basic Example

The example below demonstrates how to consume the first element of a *SafeGen* using the `next` function,
and then iterate over the remaining elements safely.

``` cpp
#include <print>

import safegen

int main() {
  // Create a SafeGen that yields 1, 2, 3
  SafeGen<int> gen{[]() -> std::generator<int> {
    co_yield 1;
    co_yield 2;
    co_yield 3;
  }};

  // Consume first element of the generator
  auto maybe_next = next(std::move(gen));

  if (maybe_next) {
    auto& [first, other] = *maybe_next;

    // Iterate over the remaining elements
    for (auto i : other) {
      std::println("{}", i);
    }

    std::println("first {}", first);
  }
}
```
