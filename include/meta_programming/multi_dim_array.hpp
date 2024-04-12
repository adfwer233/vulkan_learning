#pragma once

#include <array>

#include "type_list.hpp"

namespace MetaProgramming {

    namespace details {
        template<typename T, size_t dim, size_t... rest_dim>
        struct MultiDimensionalArrayImpl{
            using type = std::conditional<sizeof...(rest_dim) == 0, std::array<T, dim>, std::array<typename MultiDimensionalArrayImpl<T, rest_dim...>::type, dim>>;
        };
    }

    template<typename T, size_t... dim>
    struct MultiDimensionalArray {
        using type = details::MultiDimensionalArrayImpl<T, dim...>;
    };
}