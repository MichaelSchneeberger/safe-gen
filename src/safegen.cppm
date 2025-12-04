module;

#include <algorithm>
#include <functional>
#include <generator>
#include <iterator>
#include <cassert>
#include <optional>
#include <ranges>
#include <concepts>

export module safegen;

// SafeGen is a wrapper around std::generator that enforces safe usage patterns.
//
// Motivation:
//   - Using std::generator with closures that capture values is unsafe, because the lifetime
//     of the captured values is tied to the closure object, not to the generator itself. If
//     the closure is destroyed while the generator is still alove, iteration will access dangling
//     references.
//   - SafeGen solves this by storing both the generator-producing closure and the constructed
//     generator together in one owning object. This guarantees that captured values stay alive for
//     as long as the generator is alive.
//
export
template <typename T>
class SafeGen {
private:
  // closure returns a generator, which is by definition move-only
  std::move_only_function<std::generator<T>()> _closure;
  std::optional<std::generator<T>> _gen;

  // State flags for runtime safety checks, it enforces the following:
  // A Safe Generator cannot simultaneously be moved from and have being() called on it at the same time
  bool is_moved;
  bool is_begin_called;
public:
  SafeGen(std::move_only_function<std::generator<T>()> f) 
    : _closure(std::move(f)), is_moved(false), is_begin_called(false) {}

  // Delete Copy constructor
  SafeGen(const SafeGen & other) = delete;
  SafeGen & operator=(const SafeGen & other) = delete;

  SafeGen(SafeGen&& other) noexcept
    : _closure(std::move(other._closure)), is_moved(false), is_begin_called(false)
  {
    assert(!other.is_moved && !other.is_begin_called);
    other.is_moved = true;
  }

  SafeGen& operator=(SafeGen&& other) noexcept {
    assert(!other.is_moved && !other.is_begin_called);
    other.is_moved = true;

    // move fields
    _closure = std::move(other._closure); is_moved = false; is_begin_called = false;

    return *this;
  }

  auto begin() { 
    assert(!is_moved);

    is_begin_called = true;
    _gen = _closure();
    return _gen.value().begin();
  }
  std::default_sentinel_t end() const { return {}; }
};

namespace safegen {

template<typename T>
inline constexpr bool is_generator = false;

template<typename T>
inline constexpr bool is_generator<std::generator<T>> = true;

template<typename>
inline constexpr bool is_safe_gen = false;

template<typename T>
inline constexpr bool is_safe_gen<SafeGen<T>> = true;

template<typename R>
concept AllowedInput =
    (is_safe_gen<std::remove_cvref_t<R>> ||
     (std::ranges::input_range<R> &&
      !is_generator<std::remove_cvref_t<R>>));

// Returns a SafeGen that yields adjacent pairs of elements from the input generator.
//
// Example: if th einput generator yields [1, 2, 3, 4]
//          the resulting generator yields [(1,2), (2,3), (3,4)]
//
// export
// template <typename T>
// SafeGen<std::pair<T, T>> pairwise(SafeGen<T> && source) {
export
template <AllowedInput R>
auto pairwise(R && source)
  -> SafeGen<std::pair<
    std::ranges::range_value_t<R>,
    std::ranges::range_value_t<R>
  >>
{
  using T = std::ranges::range_value_t<R>;

  auto it = source.begin();
  auto end = source.end();

  if (it == end) {
    auto empty_gen = []() -> std::generator<std::pair<T, T>> { co_return; };
    return {empty_gen};
  }

  // Otherwise, store the first element
  auto first = *it;
  ++it;

  // Define a generator lambda that yields consecutive pairs
  auto gen_pairs = [it = std::move(it), first, end]() mutable -> std::generator<std::pair<T, T>> {
    for (; it != end; ++it) {
      auto next = *it;
      co_yield {first, next};
      first = next;
    }
  };

  return {std::move(gen_pairs)};
}

// Consumes the first element of the Safe Generator and returns it along with the remainder of the sequence.
//
// Returns:
//  - std::nulopt if the input generator is empty
//  - otherwise, a std::tuple containing
//      (first_element, SafeGen<T> yielding the remaining elements)
//
export
template <AllowedInput R>
auto next(R && source)
-> std::optional<std::tuple<std::ranges::range_value_t<R>, SafeGen<std::ranges::range_value_t<R>>>>
{

  using T = std::ranges::range_value_t<R>;

  auto it = source.begin();
  auto end = source.end();

  // If no elements exist, return an empty optional
  if (it == end) {
    return std::nullopt;
  }

  // Otherwise, store the first element
  auto first = *it;
  ++it;

  // Define a generator that yields the remaining elements
  auto rest_gen = [it = std::move(it), end]() mutable -> std::generator<T> {
    for (; it != end; ++it) co_yield *it;
  };

  auto safe_gen = SafeGen<T>{std::move(rest_gen)};

  return std::make_optional(std::tuple<T, SafeGen<T>>{first, std::move(safe_gen)});
}

}
