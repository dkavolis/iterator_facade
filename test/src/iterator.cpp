#include <catch2/catch.hpp>
#include <iterator_facade/iterator_facade.hpp>

namespace iterator_facade {
struct options {
  bool increment = true;
  bool equality = true;
  bool decrement = true;
  bool distance_to = true;
  bool advance = true;
  bool nothrow = true;
};

constexpr static options input_options{
    .equality = false,
    .decrement = false,
    .distance_to = false,
    .advance = false,
};

constexpr static options forward_options{
    .decrement = false,
    .distance_to = false,
    .advance = false,
};

constexpr static options bidirectional_options{
    .distance_to = false,
    .advance = false,
};

constexpr static options random_access_options{
    .increment = false,
    .equality = false,
    .decrement = false,
};

struct sentinel {
  std::int64_t i = 0;
};

struct wrapper {
  std::int64_t i = 0;
};

template <options Options = random_access_options>
struct iterator : iterator_facade<iterator<Options>> {
  wrapper value{};

  [[nodiscard]] constexpr auto dereference() const noexcept(Options.nothrow) -> wrapper { return value; }
  constexpr void increment() noexcept(Options.nothrow) { ++value.i; }

  [[nodiscard]] constexpr auto equals(iterator rhs) const noexcept(Options.nothrow) -> bool requires(Options.equality) {
    return value.i == rhs.value.i;
  }
  [[nodiscard]] constexpr auto equals(sentinel rhs) const noexcept(Options.nothrow) -> bool requires(Options.equality) {
    return value.i == rhs.i;
  }

  constexpr void decrement() noexcept(Options.nothrow) requires(Options.decrement) { --value.i; }

  [[nodiscard]] constexpr auto distance_to(iterator rhs) const noexcept(Options.nothrow) -> std::ptrdiff_t
      requires(Options.distance_to) {
    return rhs.value.i - value.i;
  }
  [[nodiscard]] constexpr auto distance_to(sentinel rhs) const noexcept(Options.nothrow) -> std::ptrdiff_t
      requires(Options.distance_to) {
    return rhs.i - value.i;
  }

  constexpr void advance(std::ptrdiff_t delta) noexcept(Options.nothrow) requires(Options.advance) { value.i += delta; }
};

struct contiguous : iterator_facade<contiguous, true> {
  using Iter = int const*;
  using reference = std::iter_reference_t<Iter>;
  using difference_type = std::iter_difference_t<Iter>;

  Iter ptr;

  [[nodiscard]] constexpr auto dereference() const noexcept -> reference { return *ptr; }
  constexpr void increment() noexcept { ++ptr; }
  [[nodiscard]] constexpr auto equals(contiguous rhs) const noexcept -> bool { return ptr == rhs.ptr; }
  constexpr void decrement() noexcept { --ptr; }
  [[nodiscard]] constexpr auto distance_to(contiguous rhs) const noexcept -> difference_type { return rhs.ptr - ptr; }
  constexpr void advance(difference_type delta) noexcept { ptr += delta; }
};

TEST_CASE("Iterator concepts", "[concepts]") {
  SECTION("Input iterator") {
    using It = iterator<input_options>;

    STATIC_REQUIRE(std::movable<It>);
    STATIC_REQUIRE(iterator_facade_subclass<It>);
    STATIC_REQUIRE(std::same_as<std::iterator_traits<It>::difference_type, std::ptrdiff_t>);
    STATIC_REQUIRE(std::same_as<std::iterator_traits<It>::value_type, wrapper>);

    STATIC_REQUIRE(std::weakly_incrementable<It>);
    STATIC_REQUIRE(std::input_or_output_iterator<It>);

    STATIC_REQUIRE(std::same_as<std::iterator_traits<It>::iterator_category, std::input_iterator_tag>);
    STATIC_REQUIRE(std::same_as<std::iterator_traits<It>::iterator_concept, std::input_iterator_tag>);
    STATIC_REQUIRE(std::input_iterator<It>);
    STATIC_REQUIRE(!std::forward_iterator<It>);
    STATIC_REQUIRE(!std::bidirectional_iterator<It>);
    STATIC_REQUIRE(!std::random_access_iterator<It>);
    STATIC_REQUIRE(!std::contiguous_iterator<It>);
  }

  SECTION("Forward iterator") {
    using It = iterator<forward_options>;

    STATIC_REQUIRE(std::equality_comparable<It>);
    STATIC_REQUIRE(std::same_as<std::iterator_traits<It>::iterator_category, std::forward_iterator_tag>);
    STATIC_REQUIRE(std::same_as<std::iterator_traits<It>::iterator_concept, std::forward_iterator_tag>);
    STATIC_REQUIRE(std::input_iterator<It>);
    STATIC_REQUIRE(std::forward_iterator<It>);
    STATIC_REQUIRE(!std::bidirectional_iterator<It>);
    STATIC_REQUIRE(!std::random_access_iterator<It>);
    STATIC_REQUIRE(!std::contiguous_iterator<It>);
  }

  SECTION("Bidirectional iterator") {
    using It = iterator<bidirectional_options>;

    STATIC_REQUIRE(std::weakly_incrementable<It>);
    STATIC_REQUIRE(_ifacade_detail::equality_comparable<It>);
    STATIC_REQUIRE(std::same_as<std::iterator_traits<It>::iterator_category, std::bidirectional_iterator_tag>);
    STATIC_REQUIRE(std::same_as<std::iterator_traits<It>::iterator_concept, std::bidirectional_iterator_tag>);
    STATIC_REQUIRE(std::input_iterator<It>);
    STATIC_REQUIRE(std::forward_iterator<It>);
    STATIC_REQUIRE(std::bidirectional_iterator<It>);
    STATIC_REQUIRE(!std::random_access_iterator<It>);
    STATIC_REQUIRE(!std::contiguous_iterator<It>);

    STATIC_REQUIRE(std::sentinel_for<sentinel, It>);
  }

  SECTION("Random access iterator") {
    using It = iterator<random_access_options>;

    STATIC_REQUIRE(std::same_as<std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>);
    STATIC_REQUIRE(std::same_as<std::iterator_traits<It>::iterator_concept, std::random_access_iterator_tag>);
    STATIC_REQUIRE(std::input_iterator<It>);
    STATIC_REQUIRE(std::forward_iterator<It>);
    STATIC_REQUIRE(std::bidirectional_iterator<It>);
    STATIC_REQUIRE(std::random_access_iterator<It>);
    STATIC_REQUIRE(!std::contiguous_iterator<It>);

    STATIC_REQUIRE(std::sentinel_for<sentinel, It>);
  }

  SECTION("Contiguous iterator") {
    using It = contiguous;

    STATIC_REQUIRE(std::same_as<std::iter_reference_t<It>, int const&>);
    STATIC_REQUIRE(std::same_as<decltype(std::declval<It>().operator->()), int const*>);
    STATIC_REQUIRE(std::same_as<std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>);
    STATIC_REQUIRE(std::same_as<std::iterator_traits<It>::iterator_concept, std::contiguous_iterator_tag>);
    STATIC_REQUIRE(std::input_iterator<It>);
    STATIC_REQUIRE(std::forward_iterator<It>);
    STATIC_REQUIRE(std::bidirectional_iterator<It>);
    STATIC_REQUIRE(std::random_access_iterator<It>);

    // because of how std::pointer_traits work contiguous_iterator only works for iterator templates... WAT?
    STATIC_REQUIRE(std::contiguous_iterator<It>);

    STATIC_REQUIRE(std::sentinel_for<It, It>);

    constexpr static int array[]{1, 2, 3, 4, 5};
    constexpr static It begin{{}, &array[0]};
    constexpr static It end{{}, &array[4]};

    STATIC_REQUIRE(std::to_address(begin) == &array[0]);
    STATIC_REQUIRE(std::to_address(end) == &array[4]);
  }
}

TEST_CASE("noexcept are propagated", "[noexcept]") {
  SECTION("nothrow") {
    iterator<random_access_options> it;

    STATIC_REQUIRE(noexcept(++it));
    STATIC_REQUIRE(noexcept(it++));
    STATIC_REQUIRE(noexcept(--it));
    STATIC_REQUIRE(noexcept(it--));
    STATIC_REQUIRE(noexcept(it == it));
    STATIC_REQUIRE(noexcept(it == sentinel{}));
    STATIC_REQUIRE(noexcept(it <= sentinel{}));
    STATIC_REQUIRE(noexcept(it >= sentinel{}));
    STATIC_REQUIRE(noexcept(*it));
    STATIC_REQUIRE(noexcept(it + 5));
    STATIC_REQUIRE(noexcept(it - 5));
    STATIC_REQUIRE(noexcept(it - it));

    STATIC_REQUIRE(nothrow_dereference<iterator<random_access_options>>);
    STATIC_REQUIRE(nothrow_increment<iterator<random_access_options>>);
    STATIC_REQUIRE(nothrow_equals<iterator<random_access_options>>);
    STATIC_REQUIRE(nothrow_decrement<iterator<random_access_options>>);
    STATIC_REQUIRE(nothrow_advance<iterator<random_access_options>>);
    STATIC_REQUIRE(nothrow_distance_to<iterator<random_access_options>>);
  }

  SECTION("throw") {
    constexpr options throw_options{.nothrow = false};
    iterator<throw_options> it;

    STATIC_REQUIRE_FALSE(noexcept(++it));
    STATIC_REQUIRE_FALSE(noexcept(it++));
    STATIC_REQUIRE_FALSE(noexcept(--it));
    STATIC_REQUIRE_FALSE(noexcept(it--));
    STATIC_REQUIRE_FALSE(noexcept(it == it));
    STATIC_REQUIRE_FALSE(noexcept(it == sentinel{}));
    STATIC_REQUIRE_FALSE(noexcept(it <= sentinel{}));
    STATIC_REQUIRE_FALSE(noexcept(it >= sentinel{}));
    STATIC_REQUIRE_FALSE(noexcept(*it));
    STATIC_REQUIRE_FALSE(noexcept(it + 5));
    STATIC_REQUIRE_FALSE(noexcept(it - 5));
    STATIC_REQUIRE_FALSE(noexcept(it - it));

    STATIC_REQUIRE_FALSE(nothrow_dereference<iterator<throw_options>>);
    STATIC_REQUIRE_FALSE(nothrow_increment<iterator<throw_options>>);
    STATIC_REQUIRE_FALSE(nothrow_equals<iterator<throw_options>>);
    STATIC_REQUIRE_FALSE(nothrow_decrement<iterator<throw_options>>);
    STATIC_REQUIRE_FALSE(nothrow_advance<iterator<throw_options>>);
    STATIC_REQUIRE_FALSE(nothrow_distance_to<iterator<throw_options>>);
  }
}

using ra_iterator = iterator<random_access_options>;
constexpr static ra_iterator i{{}, {2}};

TEST_CASE("iterator is dereferenceable", "[dereference]") {
  STATIC_REQUIRE((*i).i == 2);
  STATIC_REQUIRE(i->i == 2);
}

TEST_CASE("iterator is equality comparable", "[equality]") {
  constexpr ra_iterator it{{}, {2}};
  STATIC_REQUIRE(it == it);
  STATIC_REQUIRE_FALSE(it != it);
  STATIC_REQUIRE(it == sentinel{2});
}

TEST_CASE("iterator is incrementable", "[increment]") {
  {
    constexpr ra_iterator it = []() {
      auto iter = i;
      return ++iter;
    }();

    STATIC_REQUIRE(it == sentinel{3});
  }

  {
    constexpr ra_iterator it = []() {
      auto iter = i;
      return iter++;
    }();

    STATIC_REQUIRE(it == sentinel{2});
  }
}

TEST_CASE("iterator is decrementable", "[decrement]") {
  {
    constexpr ra_iterator it = []() {
      auto iter = i;
      return --iter;
    }();

    STATIC_REQUIRE(it == sentinel{1});
  }

  {
    constexpr ra_iterator it = []() {
      auto iter = i;
      return iter--;
    }();

    STATIC_REQUIRE(it == sentinel{2});
  }
}

TEST_CASE("iterator is advanceable", "[advance]") {
  STATIC_REQUIRE(i + 5 == sentinel{7});
  STATIC_REQUIRE(i - 5 == sentinel{-3});

  {
    constexpr auto it = []() {
      auto iter = i;
      return iter += 5;
    }();
    STATIC_REQUIRE(it == sentinel{7});
  }

  {
    constexpr auto it = []() {
      auto iter = i;
      return iter -= 5;
    }();
    STATIC_REQUIRE(it == sentinel{-3});
  }
}

TEST_CASE("iterator is subscriptable", "[subscript]") { STATIC_REQUIRE(i[5].i == 7); }

TEST_CASE("iterators are subtractable", "[subtract]") {
  SECTION("sentinel") {
    constexpr sentinel j{5};

    STATIC_REQUIRE(j - i == 3);
    STATIC_REQUIRE(i - j == -3);
  }

  SECTION("iterator") {
    constexpr ra_iterator j = i + 3;

    STATIC_REQUIRE(j - i == 3);
    STATIC_REQUIRE(i - j == -3);
  }
}

TEST_CASE("iterators are comparable", "[subtract]") {
  SECTION("sentinel") {
    constexpr sentinel j{5};

    STATIC_REQUIRE(j >= i);
    STATIC_REQUIRE(j > i);
    STATIC_REQUIRE(i <= j);
    STATIC_REQUIRE(i < j);

    STATIC_REQUIRE_FALSE(i >= j);
    STATIC_REQUIRE_FALSE(i > j);
    STATIC_REQUIRE_FALSE(j <= i);
    STATIC_REQUIRE_FALSE(j < i);

    STATIC_REQUIRE(sentinel{2} <= i);
    STATIC_REQUIRE(sentinel{2} >= i);
    STATIC_REQUIRE_FALSE(sentinel{2} < i);
    STATIC_REQUIRE_FALSE(sentinel{2} > i);

    STATIC_REQUIRE(i <= sentinel{2});
    STATIC_REQUIRE(i >= sentinel{2});
    STATIC_REQUIRE_FALSE(i < sentinel{2});
    STATIC_REQUIRE_FALSE(i > sentinel{2});
  }

  SECTION("iterator") {
    constexpr ra_iterator j = i + 3;

    STATIC_REQUIRE(j >= i);
    STATIC_REQUIRE(j > i);
    STATIC_REQUIRE(i <= j);
    STATIC_REQUIRE(i < j);

    STATIC_REQUIRE_FALSE(i >= j);
    STATIC_REQUIRE_FALSE(i > j);
    STATIC_REQUIRE_FALSE(j <= i);
    STATIC_REQUIRE_FALSE(j < i);

    STATIC_REQUIRE(i <= i);
    STATIC_REQUIRE(i >= i);
    STATIC_REQUIRE_FALSE(i < i);
    STATIC_REQUIRE_FALSE(i > i);
  }
}

}  // namespace iterator_facade
