#include <generator>
#include <print>

import safegen;

int main() {
  // Create a SafeGen that yields 1, 2, 3
  SafeGen<int> gen{[]() -> std::generator<int> {
    co_yield 1;
    co_yield 2;
    co_yield 3;
  }};

  // Consume first element of the generator
  auto maybe_next = safegen::next(std::move(gen));

  if (maybe_next) {
    auto& [first, other] = *maybe_next;

    // Iterate over the remaining elements
    for (auto i : other) {
      std::println("{}", i);
    }

    std::println("first {}", first);
  }
}
