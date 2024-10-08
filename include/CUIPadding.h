#pragma once

namespace CUI
{
    struct Padding
    {
        int top_{0};
        int right_{0};
        int bottom_{0};
        int left_{0};

        bool isTop() const;
        bool isRight() const;
        bool isBottom() const;
        bool isLeft() const;
    };
};