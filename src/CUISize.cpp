#include "CUISize.h"

bool CUI::Size::operator==(const Size &other) const
{
    return width_ == other.width_ && height_ == other.height_;
}

bool CUI::Size::operator!=(const Size &other) const
{
    return !(*this == other);
}

bool CUI::Size::operator<(const Size &other) const
{
    if (width_ != other.width_)
    {
        return width_ < other.width_;
    }

    return height_ < other.height_;
}

bool CUI::Size::operator>(const Size &other) const
{
    return other < *this;
}

bool CUI::Size::operator<=(const Size &other) const
{
    return !(*this > other);
}

bool CUI::Size::operator>=(const Size &other) const
{
    return !(*this < other);
}

CUI::Size::operator bool() const
{
    return width_ || height_;
}

CUI::Size CUI::Size::operator+(const CUI::Size &other) const
{
    return Size{width_ + other.width_, height_ + other.height_};
}

CUI::Size CUI::Size::operator-(const CUI::Size &other) const
{
    return Size{width_ - other.width_, height_ - other.height_};
}

CUI::Size &CUI::Size::operator+=(const CUI::Size &other)
{
    width_ += other.width_;
    height_ += other.height_;

    return *this;
}

CUI::Size &CUI::Size::operator-=(const CUI::Size &other)
{
    width_ -= other.width_;
    height_ -= other.height_;

    return *this;
}