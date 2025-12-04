#include <ranges>
#include <print>

import safegen;

int main() {
  auto source = std::views::iota(0, 4);
  
  // Consume first element of the generator
  auto maybe_next = safegen::next(source);

  if (maybe_next) {
    auto& [first, other] = *maybe_next;

    // Iterate over the remaining elements
    for (auto i : other) {
      std::println("{}", i);
    }

    std::println("{}", first);
  }
}
