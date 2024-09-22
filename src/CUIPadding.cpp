#include "CUIPadding.h"

bool CUI::Padding::isTop() const
{
    return 0 < top_ ;
}

bool CUI::Padding::isRight() const
{
    return 0 < right_;
}

bool CUI::Padding::isBottom() const
{
    return 0 < bottom_;
}

bool CUI::Padding::isLeft() const
{
    return 0 < left_;
}