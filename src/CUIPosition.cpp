#include "CUIPosition.h"

bool CUI::Position::operator==(const CUI::Position &other) const
{
    return x_ == other.x_ && y_ == other.y_;
}

bool CUI::Position::operator!=(const CUI::Position &other) const
{
    return !(*this == other);
}

bool CUI::Position::operator<(const CUI::Position &other) const
{
    return x_ < other.x_ || (x_ == other.x_ && y_ < other.y_);
}

bool CUI::Position::operator>(const CUI::Position &other) const
{
    return x_ > other.x_ || (x_ == other.x_ && y_ > other.y_);
}

bool CUI::Position::operator<=(const CUI::Position &other) const
{
    return !(*this > other);
}

bool CUI::Position::operator>=(const CUI::Position &other) const
{
    return !(*this < other);
}

CUI::Position::operator bool() const
{
    return x_ || y_;
}

CUI::Position CUI::Position::operator+(const CUI::Position &other) const
{
    return Position{x_ + other.x_, y_ + other.y_};
}

CUI::Position CUI::Position::operator-(const CUI::Position &other) const
{
    return Position{x_ - other.x_, y_ - other.y_};
}

CUI::Position &CUI::Position::operator+=(const CUI::Position &other)
{
    x_ += other.x_;
    y_ += other.y_;

    return *this;
}

CUI::Position &CUI::Position::operator-=(const CUI::Position &other)
{
    x_ -= other.x_;
    y_ -= other.y_;

    return *this;
}