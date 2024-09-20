#pragma once

#include <cstddef>

namespace CUI
{
    struct Padding
    {
        std::size_t top_{0};
        std::size_t right_{0};
        std::size_t bottom_{0};
        std::size_t left_{0};
    };
};