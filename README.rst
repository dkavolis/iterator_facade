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
.. _godbolt: https://godbolt.org/#z:OYLghAFBqd5QCxAYwPYBMCmBRdBLAF1QCcAaPECAMzwBtMA7AQwFtMQByARg9KtQYEAysib0QXACx8BBAKoBnTAAUAHpwAMvAFYTStJg1DIApACYAQuYukl9ZATwDKjdAGFUtAK4sGISQDspK4AMngMmAByPgBGmMQgZgBspAAOqAqETgwe3r7%2BQemZjgJhEdEscQnJtpj2JQxCBEzEBLk%2BfoG19dlNLQRlUbHxiSkKza3t%2BV3j/YMVVaMAlLaoXsTI7BzmAMzhyN5YANQmO25oDJupBAqn2CYaAIIPjwSYLKkGbydnB0wKCiOABU7i8LlcCEcEP8APr7YjvRiQ04AESOCIAjl48AjARAQckjgQlicAhYiQA6eGIwQQJanckmAIohkvF5vD5fTA/c4GAHA0FPcGYa5Q2HUtiCGFeBheJToH5ozHY3FHfHmJJEklM8kEKmXBGSgh0hmklk7KxPdnvT5Mb6nXn/QEgnb3J7jYheBxHQjxO0kGFUJiiY46l5HCNHVJeGK0PDIEBsp6Ro4XcaYVSpYhHJheIhHVCpP1EYjWax0o4AWjuAsJypxmEB0IUcINNIIDpd2FJjOTKcjBM1dioiqOACpZo5kDDRONO2O7hACAg8Ap6Rbw/2I8P9chDUiTRu%2B1uEQR1gwjsPWcezWzmdfnu6CJ7vb7iP7syAfW93yXA8GmCwB03w/GswxvcJIXCLBVFHDQHxTNM3kzbMADdUDwBUJQPEkGFQDMIR7E5LGsaCM1NJkWSte8j0fR5J3jGEnXiY1m1bPd22An9QNdddLXo5opyYgEWIgNjsKlGU5UwdAuOLEg7j4jgVloTgAFZeD8DgtFIVBODcMtLEvNYNm5XYeFIAhNGUlYAGtEjUikAE5kgADi4KQdg0SQNDMJykiCVSOEkXgWAkDQNFILSdL0jheAUEBIqs7TlNIOBYBgRAUFQD46HichKDQXL6ASP4jBhZ8ZVsvg6B/BKIBiazSBicIWgAT04CyiqNAB5BhaA6lLSCwFhDGAcQhvwBEHDwVDGyajNMGQPMtgsyC6iauMYnfYg2o8LAmufPAwu4VKqAMYAFAANTwTAAHceqLLSLP4QQRDEdgpBkQRFBUdQht0Lh9DGlBDJsLaEtgZg2BATA5tIOaEkqhhbKWFZCwaBKOErStxlknYUVMEjLDMDQqx6sx4rqJaGhcBh3E8Do9FCcIhkqEYgaKLIBCmPxOYybmGHmYYEiBuwad6CY2kZ/IxepmaBD6VphfZ0XbCl3m9FmZXWYWDmVgUEzNj0Z9MFW1Kgo0qKmti1RXKSSskkkVMDCMNVkdRtUDOJmwjlwQgSGInYgaODxiviIOuCWXhkq0NHSHsyQkmcrgNCSMxXLUpI1J2MwdiT6QgpC0gwrUyLot4WL4sSyzrJWdKsrWAhowIAqICK1I8rVmSA7V17hFEcQvv7361EOhFzdIO731STqLfUzSbc4Hq8xbgsRzth2nZdsb3c9FGSQgMPO5KyPo9rlL44QTBAJGOl9E4YvS/Lpe4tsGvY5shPEkkCkNACYOrlM5JCcmpJyARvIPw4DsReQ0q4XzjmlTKDcxLZXDmQCg7cconxGJdZgqQFAIFQK3GgtA6qUEakNFqzBdpz1IN1JEfUBpNRGmNCaOkpoSzmljHSi1lpvDoetIKOkto7T2hgLYOkjonR4Csc6TBLo3Xuo9RgdD%2B7vSHtIEeSgx4A0SMDIwoMfb6DwDESG98MbZCxjjPGqIiZWBJmTSsFMqY9GcBAVwmsgYs3KCLPQXMGheLSALBoKtFhyzcY0DWMs%2BbdAlorKWYSObq36EE7WAxdZ%2BKjqsdYxsgam3NipBe1s4GcE3o7Z2wBkDID3lVQ%2B3sHG%2B39iWSOpBQ7YK7kHMw59P7x3sjsHYFIBnDJGaMqBT8QBlxKTFTg1ckp1yQfAVBTcW5tw7l3fRPcSx6HUYPT6WjZCj3%2BlIiedDp5MFnqdIpHArYV10svVeeZ15HHKdvKpNSlz709kfDpp9djdJjnXFY19b4JHvkXUKkyX6lLfgleZl87IOSgTA6ZldZkINSig5BSB1klTWb8kYqFkCpFSDCVCXAnIwi5OMGEqgnY1TIfEeqlCdLUPanQhhggmGDQ4e8NhkjeCcJmtwhaqglorUEYIDaQ1RHtX2gKyyxBjpzzkRda6t0HpPTUbIDR%2BzvryB0ccnQIAdgGOMGDExZj4Do2uFYzgAB6PGRjGmk1cfEvwHj6ZBJ8WzcJwTijZCCQE7ISS1biwVlE1JMStbywaErDJvjVZa2iXkWJ6TQ0SANkbT6iqzYqqgbc1%2BdLnYsAUESo45KnIUmpZCI%2BFq/b4BaeZNpx9Om7B2D0oF38zBqXGZCqZdz4FwoxfXbFIAVl5nxegk1wRG0kB2TqvZEgDk/UNePPNp0p4z3zZbWBMyOAr2bk81AG9t6lvLZW6tdpGy1tbX8vOHbAUItICCrAYLrkTIHa/OZI7EU9uRXutFb9enXMpqi%2B5wGu2I0yM4SQQA%3D%3D
