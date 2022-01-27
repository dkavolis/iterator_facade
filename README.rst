===============
iterator_facade
===============

.. image:: https://github.com/dkavolis/iterator_facade/actions/workflows/windows.yml/badge.svg
        :target: https://github.com/dkavolis/iterator_facade/actions/workflows/windows.yml/badge.svg
        :alt: Github Workflow Windows Build iterator_facade Status

.. image:: https://github.com/dkavolis/iterator_facade/actions/workflows/linux.yml/badge.svg
        :target: https://github.com/dkavolis/iterator_facade/actions/workflows/linux.yml/badge.svg
        :alt: Github Workflow Linux Build iterator_facade Status

.. image:: https://github.com/dkavolis/iterator_facade/actions/workflows/macos.yml/badge.svg
        :target: https://github.com/dkavolis/iterator_facade/actions/workflows/macos.yml/badge.svg
        :alt: Github Workflow macOS Build iterator_facade Status

.. image:: https://readthedocs.org/projects/iterator-facade/badge/?version=latest
        :target: https://iterator-facade.readthedocs.io/en/latest/?badge=latest
        :alt: Documentation Status


iterator_facade for C++20


* Free software: CC0
* Documentation: https://iterator-facade.readthedocs.io.
* Workaround_ for `Clang bug`_


Features
--------

* ``constexpr`` support
* automatic deduction of iterator category
* specialized ``std::iterator_traits`` for iterators subclassing ``iterator_facade``
* no need to specify any other types unlike ``boost::iterator_facade``
* propagated ``noexcept``
* works with sentinels

Install
-------

Copy paste the single header into your project

Usage
-----

.. code-block:: c++

    #include <iterator>

    #include <iterator_facade/iterator_facade.hpp>

    namespace iterf = iterator_facade;

    template <std::input_or_output_iterator Iter>
    class my_iterator : public iterf:iterator_facade<my_iterator<Iter>> {
        template <std::input_or_output_iterator>
        friend class my_iterator;

    public:
        using value_type = std::iter_value_t<Iter>;
        using reference = std::iter_reference_t<Iter>;
        using difference_type = std::iter_difference_t<Iter>;

        constexpr my_iterator() noexcept(std::is_nothrow_constructible_v<Iter>) = default;
        constexpr my_iterator(Iter iter) noexcept(std::is_nothrow_move_constructible_v<Iter>) : iter_(std::move(iter)) {}

        template <class U>
            requires(std::constructible_from<Iter, U> && !std::same_as<Iter, U>)
        constexpr my_iterator(my_iterator<U> const& other) noexcept(std::is_nothrow_constructible_v<Iter, U>)
            : iter_(other.iter_) {}

        // Required:
        [[nodiscard]] constexpr auto dereference() const noexcept(iterf::nothrow_dereference<Iter>) -> reference { return *iter_; }
        constexpr void increment() noexcept(iterf::nothrow_increment<Iter>) { ++iter_; }

        // For forward iterators:
        template <std::sentinel_for<Iter> S>
        [[nodiscard]] constexpr auto equals(my_iterator<S> const& sentinel) const noexcept(iterf::nothrow_equals<Iter, S>)
            -> bool requires std::forward_iterator<Iter> {
            return iter_ == sentinel.iter_;
        }

        // For bidirectional iterators:
        constexpr void decrement() noexcept(iterf::nothrow_decrement<Iter>) requires std::bidirectional_iterator<Iter> { --iter_; }

        // For random access iterators:
        constexpr void advance(difference_type n) noexcept(iterf::nothrow_advance<Iter>) requires std::random_access_iterator<Iter> {
            iter_ += n;
        }

        template <std::sized_sentinel_for<Iter> S>
        [[nodiscard]] constexpr auto distance_to(my_iterator<S> const& sentinel) const noexcept(iterf::nothrow_distance_to<Iter, S>)
            -> difference_type requires std::random_access_iterator<Iter> {
            return sentinel.iter_ - iter_;
        }

    private:
        Iter iter_;
    };

| Namespace can be customized by setting ``ITERF_NAMESPACE`` before including the header
| ``my_iterator::dereference()`` is not required to return lvalue references. However, the value returned should not be a reference to a value owned by the iterator itself as it can result in dangling references.
| ``my_iterator::advance(difference_type)`` will be used in place of ``my_iterator::increment()`` or ``my_iterator::decrement()`` if any of them are not defined.
| ``my_iterator::distance_to(T)`` will be used in place of ``my_iterator::equals(T)`` if it is not defined.
|

``iterator_facade::iterator_facade<T>`` will provide operators based on defined subclass methods:

* ``dereference`` will enable
    * ``constexpr auto T::operator*() const noexcept(...) -> reference``
    * ``constexpr auto T::operator->() const noexcept(...) -> pointer`` (a proxy object if ``dereference`` returns a temporary)
* ``increment`` or ``advance`` will enable
    * ``constexpr auto T::operator++() noexcept(...) -> T&``
    * ``constexpr auto T::operator++(int) noexcept(...) -> T``
* ``equals`` or ``distance_to`` will enable
    * ``constexpr friend auto operator==(T const&, sentinel const&) noexcept(...) -> bool``
* ``decrement`` or ``advance`` will enable
    * ``constexpr auto T::operator--() noexcept(...) -> T&``
    * ``constexpr auto T::operator--(int) noexcept(...) -> T``
* ``distance_to`` will additionally enable
    * ``constexpr friend auto T::operator-(T const&, sized_sentinel const&) noexcept(...) -> difference_type``
    * ``constexpr friend auto T::operator-(sized_sentinel const&, T const&) noexcept(...) -> difference_type``
    * ``constexpr friend auto T::operator<=>(T const&, sentinel const&) noexcept(...)```
* ``advance`` will additionally enable
    * ``constexpr friend auto T::operator+(T, difference_type) noexcept(...) -> T``
    * ``constexpr friend auto T::operator+(difference_type, T) noexcept(...) -> T``
    * ``constexpr friend auto T::operator+=(T&, difference_type) noexcept(...) -> T&``
    * ``constexpr friend auto T::operator-(T, difference_type) noexcept(...) -> T``
    * ``constexpr friend auto T::operator-=(T&, difference_type) noexcept(...) -> T&``
    * ``constexpr auto T::operator[](difference_type) const noexcept(...) -> reference`` (will leave a dangling reference if it points to a value owned by the iterator)

Credits
-------

This package was created with cookietemple_ using Cookiecutter_.

.. _COOKIETEMPLE: https://cookietemple.com
.. _Cookiecutter: https://github.com/audreyr/cookiecutter
.. _Clang bug: https://github.com/llvm/llvm-project/issues/44178
.. _Workaround: https://stackoverflow.com/a/66392670/13262469
