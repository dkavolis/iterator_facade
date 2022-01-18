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

    struct my_iterator : iterf::iterator_facade<my_iterator> {
        using difference_type = std::iter_difference_t<iterator>;
        using reference = std::iter_reference_t<iterator>;

        iterator it;

        // Required:
        constexpr auto dereference() const noexcept(noexcept(*it)) -> reference { return *it; }
        constexpr void increment() noexcept(noexcept(*it)) { ++it; }

        // For forward iterators:
        constexpr auto equals(std::sentinel_for<iterator> auto other) const noexcept(noexcept(it == other)) -> bool { return it == other; }
        constexpr auto equals(my_iterator other) const noexcept(noexcept(it == other.it)) -> bool { return it == other.it; }

        // For bidirectional operators:
        constexpr void decrement() noexcept(noexcept(--it)) { --it; }

        // For random access iterators:
        constexpr auto distance_to(std::sentinel_for<iterator> auto lhs) const noexcept(noexcept(lhs - it)) -> difference_type { return lhs - it; }
        constexpr auto distance_to(my_iterator lhs) const noexcept(noexcept(lhs.it - it)) -> difference_type { return lhs.it - it; }
        constexpr void advance(difference_type n) noexcept(noexcept(it += n)) { it += n; }
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
    * ``constexpr friend auto T::operator-(T const&, sentinel const&) noexcept(...) -> difference_type``
    * ``constexpr friend auto T::operator(sentinel const&, T const&) noexcept(...) -> difference_type``
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
.. _Clang bug: https://bugs.llvm.org/show_bug.cgi?id=44833
.. _Workaround: https://stackoverflow.com/a/66392670/13262469
