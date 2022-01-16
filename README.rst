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
* Clang is bugged: godbolt_


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

    #include <iterator_facade/iterator_facade>

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

``iterf::iterator_facade<T>`` will provide operators based on defined subclass methods:

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
.. _godbolt: https://godbolt.org/#z:OYLghAFBqd5QCxAYwPYBMCmBRdBLAF1QCcAaPECAMzwBtMA7AQwFtMQByARg9KtQYEAysib0QXACx8BBAKoBnTAAUAHpwAMvAFYTStJg1DIApACYAQuYukl9ZATwDKjdAGFUtAK4sGISRqkrgAyeAyYAHI%2BAEaYxCBmZqQADqgKhE4MHt6%2B/oGp6Y4CoeFRLLHxibaY9kUMQgRMxATZPn4B1bWZDU0EJZExcQlJCo3Nrbkdo739ZRXDAJS2qF7EyOwc5gDMYcjeWADUJltuaAzryQQKx9gmGgCCd/cEmCzJBi9HJ3tMCgoHABUbk8zhcCAcEL8APq7YivRjg44AEQOcIAjl48HD/hAgWYAGwHAgLI4AdgsRIAdLD4YIIAtjhSTKSkYynk8Xm8PpgvqcDH9AcCHqNiF4HAdCHEmERiFCqExRIdmVYHgc1Qdkl5orQ8MgQOzVeqzqNMKpksQDkwvEQDqhklKZdZrPSDgBaG6CgmozAYrGYf6QhQw85wtiCY5uIFbbBkpmG9XqvGEuxUL4ogBU00cyCholGEYB6ZuEAICDwCgZWxV9wTCZT1JDtII9LZ8drcIIqwYBxTrZr6uZrIeg77TxFYvBkuI0pIBxAEpe05lcoVTCwEanM%2BIHuVTwTYUnDCwqjTBw0fYTxpeZotADdUHh0BLG2HmySGKhTWDY0dLNYwsejJkkOjwsqOwqNNmUK/EozQQIGwbIKGCIbouW43JWFgcEstCcAArLwfgcFopCoJwbhOpYPYrGsPLbDwpAEJo2FLAA1gkeGUgAnASAAcXBSFsGgBGYXH4qS%2BicJIvAsBIGiBERJFkRwvAKCAgRMcR2GkHAsAwIgKCoG8dBxOQlBoMZ9DxD8RhQgQooMKxfB0IuakQNEzGkNEYRNAAnpwDEWa%2BADyDC0P5WmkFgLCGMA4iRfgcIOHgt7%2Bp5pqYMg1obAxB41J5OrRNOxC%2BR4WCefZeCydw2lUAYwAKAAangmAAO7BfaREMfwggiGI7BSDIgiKCo6iRboXD6LFKCUTYhVqbAzBsCAmCpaQqXxPZXiOQsSx2nUakcK6rqjOgyKmH%2BlhmBobrBWYqk1JldQuEeEx%2BJNIRhAM5RDJNBQZAIb16P9dSzIM8STXYT3dGMLSeG0ehQ8lAg9M0YM/RDtiw0DkOw%2Bj8xcEsCg0esej2ZgOXabhHAEaQim8Mpqi8firr4pIBw2cABwlg5rEkhAFGXTYBy4IQs70aQBweJZcS/lshO8JpWi7aQ7GSPi3FcBo%2BJmLxeH4nhWxmFs6vSNT0mkLJeEKZ5ymqepjHMUsukGSsBCagQZkQBZyQmZjmD4DKeg9cIojiINIcjWoFVwpTpCtdOyQBVT%2BGEbbnDBdaHu2qmTMs2zHMGEY3NbTt3PS77VlywrjtaSrCCYGuQz0pJHAW1bNuRXbtgO0rLGqwkkiUhopLy7xev4lxeFcaSwmt1sadd5witOzp%2Bku/Bhky2QFDe0ZldDMAAlJDQtCuZQHmRd5zAlcnpBBQioXhZ50WxfFJGJdDqWHSRGVZS8d88rUxIoVYqpUMAbBIpVaqPAlh1SYA1ZqbUOqMDviHPq4dpCRyUNHcaCQppGBmkLfQeBogLRbvtTIh1jqnXOrNa6t17qdGhs4CArggaSAkp9Uo4NgZpABlkeGkwJIg0yPjX6zDkb1GxkI9oEkkZ1FRn0L6cwJHTHGLI/w8i8YqN4YTZYqxSaTXJpTHCqc6bpw4HnVm7NgDIGQAcASlIzDl1mpLUWMpq6Swrn7OWZgFgrzrmxEAWwtiUlCREyJUTW7txANbCxS8VI9w0qvDeSA3Yey9j7P2%2BCA5i0xugsOA0sGyCjmNKBsc74JyYEnGqZiaaLyUhnLO1oc4HGsQXOxDinEuIFvvXx2x/GBOVksBuTd4gt3NjJOJncmlJLUikoJA8zB4Xno0hmy9a7KzXvANJW8D473Mv0qyIBbzIGSMkKEt4uBcShNyUYUJVBs2cmfOIblL4kWvn5O%2BD9BBPwih/V4b9IG8E/slb%2B6VVCZWyoAwQ%2BVIqgL8mVEFjFiBVWTnA%2BqTUWrtU6mg2QGDilDXkDg8pOgQkEOMG4%2Ba8A9qXCoZwAA9KdIhVgrokQUZkF67hNEfSPOIzGojAa8pSPw0GuiMaI0elIpRONJGKJ0TwyVuNehyvUcopVBMiYkwGqiimGLW603pqRTgTz2YsAUGcg41yuKUnueCAWbiRaB3FsbSaUtjmy22FsAJWz%2B7sRWTE6Z8TjXdwWX652690ktM9rvbJJythBBdQUglRSJAlOGqSmO%2Bqarx0Tga6mRrLGZ3dq01AucC4WqtTau10p/QOp8VXb1vq%2B710blgCZ9TYkhssfbRZIzlmrOpgvBJczhn92pkw0NmzW1LA2ukZwkggA
