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

  // Create a new SafeGen that yields
  auto one_added = add_one(std::move(source));

  // Consume first element of the generator
  auto maybe_next = safegen::next(std::move(one_added));

  if (maybe_next) {
    auto& [first, other] = *maybe_next;

    // Iterate over the remaining elements
    for (auto i : other) {
      std::println("{}", i);
    }

    std::println("{}", first);
  }
}
