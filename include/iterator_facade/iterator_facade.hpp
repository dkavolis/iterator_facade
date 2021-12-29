// iterator_facade by D. Kavolis
//
// To the extent possible under law, the person who associated CC0 with
// iterator_facade has waived all copyright and related or neighboring rights
// to iterator_facade.
//
// You should have received a copy of the CC0 legalcode along with this
// work.  If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

#pragma once

#include <concepts>
#include <iterator>
#include <type_traits>

#if defined(_MSC_VER)
#  define ITERF_ALWAYS_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#  define ITERF_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#  define ITERF_ALWAYS_INLINE inline
#endif

#ifndef ITERF_NAMESPACE
#  define ITERF_NAMESPACE iterf
#endif

// https://vector-of-bool.github.io/2020/06/13/cpp20-iter-facade.html
namespace ITERF_NAMESPACE {

namespace detail {

/// \brief Wrapper for rvalue return values when expecting an lvalue address
/// \tparam T
template <class T>
struct arrow_proxy {
  T object;

  [[nodiscard]] ITERF_ALWAYS_INLINE constexpr auto operator->() noexcept -> T* { return &object; }
  [[nodiscard]] ITERF_ALWAYS_INLINE constexpr auto operator->() const noexcept -> T const* { return &object; }

  [[nodiscard]] ITERF_ALWAYS_INLINE constexpr auto operator*() noexcept -> T& { return object; }
  [[nodiscard]] ITERF_ALWAYS_INLINE constexpr auto operator*() const noexcept -> T const& { return object; }

  // NOLINTNEXTLINE(google-explicit-constructor)
  [[nodiscard]] ITERF_ALWAYS_INLINE constexpr operator T*() noexcept { return &object; }
  // NOLINTNEXTLINE(google-explicit-constructor)
  [[nodiscard]] ITERF_ALWAYS_INLINE constexpr operator T const*() const noexcept { return &object; }
};

template <typename T>
struct inferred_value_type {
  using type = std::remove_cvref_t<decltype(*std::declval<T&>())>;
};
template <typename T>
  requires requires { typename T::value_type; }
struct inferred_value_type<T> {
  using type = typename T::value_type;
};
template <typename T>
using inferred_value_type_t = typename inferred_value_type<T>::type;

// clang-format off

// Check for .increment
template <typename T>
concept has_increment = requires(T& it) {
  it.increment();
};
template <typename T>
concept has_nothrow_increment = requires(T& it) {
  { it.increment() } noexcept;
};

// Check for .decrement
template <typename T>
concept has_decrement = requires(T& it) {
  it.decrement();
};
template <typename T>
concept has_nothrow_decrement = requires(T& it) {
  { it.decrement() } noexcept;
};

// Check for .distance_to
template <typename T, typename It = T>
concept has_distance_to = requires(It const& it, T const& other) {
  it.distance_to(other);
};
template <typename T, typename It = T>
concept has_nothrow_distance_to = requires(It const& it, T const& other) {
  { it.distance_to(other) } noexcept;
};

template <typename>
struct inferred_difference_type {
  using type = std::ptrdiff_t;
};
template <has_distance_to T>
struct inferred_difference_type<T> {
  static const T& it;
  using type = decltype(it.distance_to(it));
};
template <typename T>
using inferred_difference_type_t = typename inferred_difference_type<T>::type;

// Check for .advance
template <typename T, class Diff = inferred_difference_type_t<T>>
concept has_advance = requires(T& it, Diff offset) {
  it.advance(offset);
};
template <typename T, class Diff = inferred_difference_type_t<T>>
concept has_nothrow_advance = requires(T& it, Diff offset) {
  { it.advance(offset) } noexcept;
};

// Check for equals
template <typename T, typename It = T>
concept equality_comparable = requires(const T& sentinel, const It& it) {
  { it.equals(sentinel) } -> std::convertible_to<bool>;
};

template <typename T, typename It = T>
concept nothrow_equality_comparable = requires(const T& sentinel, const It& it) {
  { it.equals(sentinel) } noexcept -> std::convertible_to<bool>;
};

template <class T>
concept has_nothrow_dereference = requires(T const& it) {
  { it.dereference() } noexcept;
};

// We can meet "random access" if it provides
// both .advance() and .distance_to()
template <typename T>
concept meets_random_access = has_advance<T> && has_distance_to<T>;

// We meet `bidirectional` if we are random_access, OR we have .decrement()
template <typename T>
concept meets_bidirectional = meets_random_access<T> || has_decrement<T>;

template <typename T>
concept decls_contiguous = requires {
  { T::contiguous_iterator } -> std::convertible_to<bool>;
} && T::contiguous_iterator;

template <typename Arg, typename Iter>
concept difference_type_arg = std::convertible_to<Arg, inferred_difference_type_t<Iter>>;

template <typename Arg, typename Iter>
concept advance_type_arg = difference_type_arg<Arg, Iter> && has_advance<Iter, Arg>;

template <class T>
concept incrementable = has_increment<T> || has_advance<T> || requires(T& it) {
  { ++it } -> std::common_reference_with<std::remove_cvref_t<T>>;
};
// clang-format on

template <class Iter>
using iterator_category_t =
    std::conditional_t<meets_random_access<Iter>,
                       // We meet the requirements of random-access:
                       std::random_access_iterator_tag,
                       // We don't:
                       std::conditional_t<meets_bidirectional<Iter>,
                                          // We meet requirements for bidirectional usage:
                                          std::bidirectional_iterator_tag,
                                          // We don't:
                                          std::conditional_t<equality_comparable<Iter>,
                                                             // equality equality_comparable satisfies forward iterator
                                                             std::forward_iterator_tag,
                                                             // Otherwise we are an input iterator:
                                                             std::input_iterator_tag>>>;

// contiguous_iterator is a special case of random_access and output iterator is deduced by STL
template <class Iter>
using iterator_concept_t =
    std::conditional_t<decls_contiguous<Iter>, std::contiguous_iterator_tag, iterator_category_t<Iter>>;

template <class T>
[[nodiscard]] ITERF_ALWAYS_INLINE constexpr auto arrow_helper(T& t) noexcept -> T& {
  return t;
}
template <class T>
  requires(!std::is_lvalue_reference_v<T>)
[[nodiscard]] ITERF_ALWAYS_INLINE constexpr auto arrow_helper(T&& t) noexcept(
    std::is_nothrow_move_constructible_v<std::remove_cvref_t<T>>) -> arrow_proxy<std::remove_cvref_t<T>> {
  return {std::move(t)};
}

}  // namespace detail

/** @defgroup facade Iterator facade
 *  @{
 */

/**
 * @brief Iterator facade which infers iterator types and functionality and propagates <code>noexcept</code>
 * @tparam Derived iterator subclass type which implements: <br>
 *
 *    Input iterator (required): <br>
 *    *   <code>auto dereference() const -> reference </code> <br>
 *    *   <code>void increment() </code> <br>
 *
 *    Forward: <br>
 *    *   <code>auto equals(T|sentinel) const -> bool </code> <br>
 *
 *    Bidirectional: <br>
 *    *   <code>void decrement() </code> <br>
 *
 *    Random access: <br>
 *    *   <code>auto distance_to(T|sentinel) const -> difference_type </code> (can replace equal) <br>
 *    *   <code>void advance(difference_type) </code> (can replace increment/decrement) <br>
 *
 * @tparam Contiguous true if the derived iterator is contiguous, otherwise false since it cannot be inferred
 */
template <typename Derived, bool Contiguous = false>
class iterator_facade {
 public:
  using self_type = Derived;

  constexpr static bool contiguous_iterator = Contiguous;

  // cannot add any type aliases as Derived is incomplete at this point, can only rely on decltype(auto) in declarations

 private:
  friend Derived;
  [[nodiscard]] ITERF_ALWAYS_INLINE constexpr auto self() noexcept -> self_type& {
    return static_cast<self_type&>(*this);
  }
  [[nodiscard]] ITERF_ALWAYS_INLINE constexpr auto self() const noexcept -> const self_type& {
    return static_cast<const self_type&>(*this);
  }

 public:
  /** @defgroup dereference Dereferencing
   *  Requires <code>Derived::dereference() const</code>
   *  @{
   */

  /**
   * @brief Dereference operator
   *
   * @return decltype(Derived{}.dereference())
   */
  [[nodiscard]] ITERF_ALWAYS_INLINE constexpr auto operator*() const
      noexcept(detail::has_nothrow_dereference<self_type>) -> decltype(auto) {
    return self().dereference();
  }

  /**
   * @brief Arrow operator
   *
   * @return Pointer or arrow proxy to the return value of <code>Derived::dereference() const</code>
   */
  [[nodiscard]] ITERF_ALWAYS_INLINE constexpr auto
  operator->() const noexcept((detail::has_nothrow_dereference<self_type>&& noexcept(detail::arrow_helper(**this))))
      -> decltype(auto) {
    return detail::arrow_helper(**this);
  }

  /** @} */  // end of dereference

  /** @defgroup equality Equality comparisons
   *  Requires <code>Derived::equals(T) const</code> or <code>Derived::distance_to(T) const</code>
   *  @{
   */

  /**
   * @brief Equality comparison operator, the default overload which requires <code>Derived::equals(T) const</code>
   *
   * @tparam T comparison type
   * @param lhs
   * @param rhs
   * @return true if lhs == rhs
   * @return false otherwise
   */
  template <detail::equality_comparable<self_type> T>
  [[nodiscard]] ITERF_ALWAYS_INLINE constexpr auto friend operator==(self_type const& lhs, T const& rhs) noexcept(
      detail::nothrow_equality_comparable<T, self_type>) -> bool {
    return lhs.equals(rhs);
  }

  /**
   * @brief Fallback equality comparison operator when <code>Derived::equals(T) const</code> is not available, but
   * <code>Derived::distance_to(T) const</code> is
   *
   * @tparam T comparison type
   * @param lhs
   * @param rhs
   * @return true if lhs == rhs
   * @return false otherwise
   */
  template <detail::has_distance_to<self_type> T>
#ifndef ITERF_DOXYGEN_RUNNING
    requires(!detail::equality_comparable<T, self_type>)
#endif
  [[nodiscard]] ITERF_ALWAYS_INLINE constexpr auto friend operator==(self_type const& lhs, T const& rhs) noexcept(
      detail::has_nothrow_distance_to<T, self_type>) -> bool {
    return lhs.distance_to(rhs) == 0;
  }

  /** @} */  // end of equality

  /** @defgroup increment Increment operators
   *  Requires <code>Derived::increment()</code> or <code>Derived::advance(1)</code> to be valid
   *  @{
   */

  /**
   * @brief Default pre-increment operator, requires <code>Derived::increment()</code>
   *
   * @return Derived&
   */
  ITERF_ALWAYS_INLINE constexpr auto operator++() noexcept(detail::has_nothrow_increment<self_type>)
      -> self_type& requires detail::has_increment<self_type> {
    self().increment();
    return self();
  }

  /**
   * @brief Fallback pre-increment operator when <code>Derived::increment()</code> is not available, requires
   * <code>Derived::advance(1)</code> to be valid
   *
   * @return Derived&
   */
  ITERF_ALWAYS_INLINE constexpr auto operator++() noexcept(detail::has_nothrow_advance<self_type, int>)
      -> self_type& requires(!detail::has_increment<self_type> && detail::has_advance<self_type, int>) {
    self().advance(1);
    return self();
  }

  /**
   * @brief Post-increment operator, requires <code>Derived::increment()</code> or <code>Derived::advance(1)</code>
   *
   * @return Derived&
   */
  [[nodiscard]] constexpr auto operator++(int) noexcept(std::is_nothrow_copy_constructible_v<self_type>&& noexcept(
      ++(*this))) -> self_type requires(detail::has_increment<self_type> || detail::has_advance<self_type, int>) {
    auto copy = self();
    ++(*this);
    return copy;
  }

  /** @} */  // end of increment

  /** @defgroup decrement Decrement operators
   *  Requires <code>Derived::decrement()</code> or <code>Derived::advance(-1)</code> to be valid
   *  @{
   */

  /**
   * @brief Default pre-decrement operator, requires <code>Derived::decrement()</code>
   *
   * @return Derived&
   */
  ITERF_ALWAYS_INLINE constexpr auto operator--() noexcept(detail::has_nothrow_decrement<self_type>)
      -> self_type& requires detail::has_decrement<self_type> {
    self().decrement();
    return self();
  }

  /**
   * @brief Fallback pre-decrement operator when <code>Derived::decrement()</code> is not available, requires
   * <code>Derived::advance(-1)</code> to be valid
   *
   * @return Derived&
   */
  ITERF_ALWAYS_INLINE constexpr auto operator--() noexcept(detail::has_nothrow_advance<self_type, int>)
      -> self_type& requires(!detail::has_decrement<self_type> && detail::has_advance<self_type, int>) {
    self().advance(-1);
    return self();
  }

  /**
   * @brief Post-decrement operator, requires <code>Derived::decrement()</code> or <code>Derived::advance(-1)</code>
   *
   * @return Derived&
   */
  [[nodiscard]] constexpr auto operator--(int) noexcept(std::is_nothrow_copy_constructible_v<self_type>&& noexcept(
      --(*this))) -> self_type requires(detail::has_decrement<self_type> || detail::has_advance<self_type, int>) {
    auto copy = self();
    ++(*this);
    return copy;
  }

  /** @} */  // end of decrement

  /** @defgroup operators Operators
   *  Requires <code>Derived::advance(difference_type)</code>
   *  @{
   */

  template <detail::advance_type_arg<self_type> D>
  ITERF_ALWAYS_INLINE friend constexpr auto operator+=(self_type& self,
                                                       D offset) noexcept(detail::has_nothrow_advance<self_type, D>)
      -> self_type& {
    self.advance(offset);
    return self;
  }

  template <detail::advance_type_arg<self_type> D>
  [[nodiscard]] ITERF_ALWAYS_INLINE friend constexpr auto operator+(self_type left, D off) noexcept(
      detail::has_nothrow_advance<self_type, D>) -> self_type {
    return left += off;
  }

  template <detail::advance_type_arg<self_type> D>
  [[nodiscard]] ITERF_ALWAYS_INLINE friend constexpr auto operator+(D off, self_type right) noexcept(
      detail::has_nothrow_advance<self_type, D>) -> self_type {
    return right += off;
  }

  template <detail::advance_type_arg<self_type> D>
  [[nodiscard]] ITERF_ALWAYS_INLINE friend constexpr auto operator-(self_type left, D off) noexcept(
      detail::has_nothrow_advance<self_type, D>) -> self_type {
    return left + -off;
  }

  template <detail::advance_type_arg<self_type> D>
  ITERF_ALWAYS_INLINE friend constexpr auto operator-=(self_type& left,
                                                       D off) noexcept(detail::has_nothrow_advance<self_type, D>)
      -> self_type& {
    return left = left - off;
  }

  template <detail::advance_type_arg<self_type> D>
  [[nodiscard]] ITERF_ALWAYS_INLINE constexpr auto operator[](D off) const
      noexcept(detail::has_nothrow_advance<self_type, D>&& detail::has_nothrow_dereference<self_type>)
          -> decltype(auto) {
    return (self() + off).dereference();
  }

  /** @} */  // end of operators

  /** @defgroup distance Iterator difference operators
   *  Requires <code>Derived::distance_to(T)</code>
   *  @{
   */

  /**
   * @brief Distance between two iterators or iterator and sentinel pair
   *
   * @tparam T Iterator/sentinel type for which <code>Derived::distance_to(T)</code> is valid
   * @param left
   * @param right
   * @return decltype(auto)
   */
  template <detail::has_distance_to<self_type> T>
  [[nodiscard]] ITERF_ALWAYS_INLINE friend constexpr auto operator-(const T& left, self_type const& right) noexcept(
      detail::has_nothrow_distance_to<T, self_type>) -> decltype(auto) {
    // Many many times must we `++right` to reach `left` ?
    return right.distance_to(left);
  }

  /**
   * @brief Distance between an iterator and a sentinel
   *
   * @tparam Sentinel Sentinel type for which <code>Derived::distance_to(Sentinel)</code> is valid
   * @param left iterator
   * @param right sentinel
   * @return decltype(auto)
   */
  template <detail::has_distance_to<self_type> Sentinel>
#ifndef ITERF_DOXYGEN_RUNNING
    requires(!std::same_as<Sentinel, self_type>)
#endif
  [[nodiscard]] ITERF_ALWAYS_INLINE friend constexpr auto operator-(
      const self_type& left, Sentinel const& right) noexcept(detail::has_nothrow_distance_to<Sentinel, self_type>)
      -> decltype(auto) {
    return -(right - left);
  }

  /** @} */  // end of distance

  /** @defgroup comparison Comparison operators
   *  Requires <code>Derived::distance_to(T)</code>
   *  @{
   */

  template <detail::has_distance_to<self_type> Sentinel>
  [[nodiscard]] ITERF_ALWAYS_INLINE friend constexpr auto operator<=>(
      const self_type& left, const Sentinel& right) noexcept(detail::has_nothrow_distance_to<Sentinel, self_type>) {
    return -left.distance_to(right) <=> 0;
  }

  /** @} */  // end of comparison
};

/** @} */  // end of facade

namespace detail {

template <class Derived>
struct is_base_of_facade {
 private:
  template <class T, bool B>
  static auto derives(iterator_facade<T, B> const&) -> std::true_type;
  static auto derives(...) -> std::false_type;

 public:
  constexpr static bool value = decltype(derives(std::declval<Derived>()))::value;
};
}  // namespace detail

/**
 * @brief Check if type is derived from \ref iterator_facade
 *
 * @tparam T type to check
 */
template <class T>
concept iterator_facade_subclass = detail::is_base_of_facade<T>::value;

}  // namespace ITERF_NAMESPACE

template <ITERF_NAMESPACE ::iterator_facade_subclass Iter>
struct std::iterator_traits<Iter> {
  using reference = decltype(*std::declval<Iter&>());
  using pointer = decltype(std::declval<Iter&>().operator->());
  using difference_type = ITERF_NAMESPACE ::detail::inferred_difference_type_t<Iter>;
  using value_type = ITERF_NAMESPACE ::detail::inferred_value_type_t<Iter>;

  using iterator_category = ITERF_NAMESPACE ::detail::iterator_category_t<Iter>;
  using iterator_concept = ITERF_NAMESPACE ::detail::iterator_concept_t<Iter>;
};
