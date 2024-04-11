#pragma once

#include "type_list.hpp"

namespace MetaProgramming {

template <size_t... idx> struct LoopIndices {
    struct IsLoopIndices {};

    constexpr static size_t size = sizeof...(idx);

    template <size_t new_idx> using append = LoopIndices<idx..., new_idx>;
};

namespace details {
template <typename T>
concept LI = requires { typename T::IsLoopIndices; };

template <size_t num> struct GetZeroLoopIndices {
    using type = GetZeroLoopIndices<num - 1>::type::template append<0>;
};

template <> struct GetZeroLoopIndices<0> {
    using type = LoopIndices<>;
};

template <typename T>
concept StaticLoopFunction = requires {
    typename T::IsStaticLoopFunction;
    { T::nested_loop_num };
    { T::loop_body(typename GetZeroLoopIndices<T::nested_loop_num>::type()) };
};

template <StaticLoopFunction Function, size_t... idx> void static_loop_invoke(LoopIndices<idx...>) {
    std::invoke(Function::template loop_body<idx...>, LoopIndices<idx...>());
}

template <LI loop_indices, StaticLoopFunction Function, typename T, T... indices, typename... Seqs>
void multi_layer_loop_inner(std::integer_sequence<T, indices...>, Seqs... seqs) {
    if constexpr (sizeof...(Seqs) == 0) {
        (static_loop_invoke<Function>(typename loop_indices::template append<indices>()), ...);
    } else {
        (multi_layer_loop_inner<typename loop_indices::template append<indices>, Function>(seqs...), ...);
    }
}
} // namespace details

template <typename T>
concept StaticLoopFunction = details::StaticLoopFunction<T>;

template <details::StaticLoopFunction Function, typename... Seqs> void static_loop(Seqs... seqs) {
    static_assert(sizeof...(seqs) == Function::nested_loop_num);
    details::multi_layer_loop_inner<LoopIndices<>, Function>(seqs...);
}

} // namespace MetaProgramming