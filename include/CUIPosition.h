#pragma once

#include <cstddef>

namespace CUI
{
    struct Position
    {
        std::size_t x_{0};
        std::size_t y_{0};

        bool operator==(const Position &other) const;
        bool operator!=(const Position &other) const;
        bool operator<(const Position &other) const;
        bool operator>(const Position &other) const;
        bool operator<=(const Position &other) const;
        bool operator>=(const Position &other) const;

        explicit operator bool() const;

        Position operator+(const Position& other) const;
        Position operator-(const Position& other) const;
        Position &operator+=(const Position &other);
        Position &operator-=(const Position &other);
    };
};