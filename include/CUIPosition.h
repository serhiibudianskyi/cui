#pragma once

namespace CUI
{
    struct Position
    {
        int x_{0};
        int y_{0};

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