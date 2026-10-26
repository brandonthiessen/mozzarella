#pragma once

#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>

namespace mozzarella {

class [[nodiscard]] Bitboard {
public:
    class Iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = int;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = int;

        constexpr Iterator() = default;

        explicit constexpr Iterator(uint64_t remaining)
            : remaining_{remaining}
        {}

        [[nodiscard]] constexpr int operator*() const
        {
            assert(remaining_ != 0);
            return std::countr_zero(remaining_);
        }

        constexpr Iterator& operator++()
        {
            assert(remaining_ != 0);
            remaining_ &= (remaining_ - 1);
            return *this;
        }

        constexpr Iterator operator++(int)
        {
            Iterator previous = *this;
            ++(*this);
            return previous;
        }

        constexpr bool operator==(const Iterator& other) const = default;

    private:
        uint64_t remaining_ = 0;
    };

    constexpr Bitboard();
    explicit constexpr Bitboard(uint64_t value);

    constexpr Bitboard& operator&=(const Bitboard& other);
    constexpr Bitboard& operator|=(const Bitboard& other);
    constexpr Bitboard& operator^=(const Bitboard& other);
    constexpr Bitboard& operator<<=(int shift);
    constexpr Bitboard& operator>>=(int shift);

    [[nodiscard]] constexpr Bitboard operator~() const;

    [[nodiscard]] explicit constexpr operator bool() const;

    [[nodiscard]] constexpr bool operator==(const Bitboard& other) const = default;


    [[nodiscard]] constexpr int pop_lsb();
    [[nodiscard]] constexpr bool empty() const;
    [[nodiscard]] constexpr int count() const;
    constexpr void set(int square);
    constexpr void clear(int square);
    [[nodiscard]] constexpr bool test(int square) const;

    [[nodiscard]] constexpr uint64_t value() const;

    constexpr Iterator begin() const;
    constexpr Iterator end() const;

private:
    uint64_t bitboard_;
};

constexpr Bitboard::Bitboard()
    : bitboard_{0}
{}

constexpr Bitboard::Bitboard(uint64_t value)
    : bitboard_{value}
{}

constexpr Bitboard&
Bitboard::operator&=(const Bitboard& other)
{
    bitboard_ &= other.bitboard_;
    return *this;
}

constexpr Bitboard&
Bitboard::operator|=(const Bitboard& other)
{
    bitboard_ |= other.bitboard_;
    return *this;
}

constexpr Bitboard&
Bitboard::operator^=(const Bitboard& other)
{
    bitboard_ ^= other.bitboard_;
    return *this;
}

constexpr Bitboard&
Bitboard::operator<<=(int shift)
{
    assert((shift >= 0) && (shift < 64));
    bitboard_ <<= shift;
    return *this;
}

constexpr Bitboard&
Bitboard::operator>>=(int shift)
{
    assert((shift >= 0) && (shift < 64));
    bitboard_ >>= shift;
    return *this;
}

constexpr Bitboard
Bitboard::operator~() const
{
    return Bitboard{~bitboard_};
}

constexpr Bitboard::operator bool() const
{
    return bitboard_ != 0;
}

constexpr int
Bitboard::pop_lsb()
{
    assert(bitboard_ != 0);
    int index = std::countr_zero(bitboard_);
    bitboard_ &= (bitboard_ - 1);
    return index;
}

constexpr bool
Bitboard::empty() const
{
    return bitboard_ == 0;
}

constexpr int
Bitboard::count() const
{
    return std::popcount(bitboard_);
}

constexpr void
Bitboard::set(int square)
{
    assert((0 <= square) && (square < 64));
    bitboard_ |= (1ULL << square);
}

constexpr void
Bitboard::clear(int square)
{
    assert((0 <= square) && (square < 64));
    bitboard_ &= ~(1ULL << square);
}

constexpr bool
Bitboard::test(int square) const
{
    assert((0 <= square) && (square < 64));
    return (bitboard_ & (1ULL << square)) != 0;
}

[[nodiscard]] constexpr Bitboard
operator&(Bitboard lhs, const Bitboard& rhs)
{
    lhs &= rhs;
    return lhs;
}

[[nodiscard]] constexpr Bitboard
operator|(Bitboard lhs, const Bitboard& rhs)
{
    lhs |= rhs;
    return lhs;
}

[[nodiscard]] constexpr Bitboard
operator^(Bitboard lhs, const Bitboard& rhs)
{
    lhs ^= rhs;
    return lhs;
}

[[nodiscard]] constexpr Bitboard
operator<<(Bitboard lhs, int shift)
{
    assert((shift >= 0) && (shift < 64));
    lhs <<= shift;
    return lhs;
}

[[nodiscard]] constexpr Bitboard
operator>>(Bitboard lhs, int shift)
{
    assert((shift >= 0) && (shift < 64));
    lhs >>= shift;
    return lhs;
}

constexpr Bitboard::Iterator
Bitboard::begin() const
{
    return Iterator{bitboard_};
}

constexpr Bitboard::Iterator
Bitboard::end() const
{
    return Iterator{0};
}

[[nodiscard]] constexpr uint64_t Bitboard::value() const
{
    return bitboard_;
}

} // namespace mozzarella
