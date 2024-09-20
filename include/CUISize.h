#pragma once

#include <cstddef>

namespace CUI
{
    struct Size
    {
        int width_{0};
        int height_{0};

        bool operator==(const Size &other) const;
        bool operator!=(const Size &other) const;
        bool operator<(const Size &other) const;
        bool operator>(const Size &other) const;
        bool operator<=(const Size &other) const;
        bool operator>=(const Size &other) const;

        explicit operator bool() const;

        Size operator+(const Size &other) const;
        Size operator-(const Size &other) const;
        Size &operator+=(const Size &other);
        Size &operator-=(const Size &other);
    };
};