#pragma once

#include <tuple>

namespace VklTypeList {

    enum { UIComponentMaxRegisteredTypes = 15 };

    template<int N>
    struct Rank : Rank<N - 1> {};

    template<>
    struct Rank<0> {};

    template<int N, typename... Types>
    using NthTypeOf = typename std::tuple_element<N, std::tuple<Types...>>::type;

    template <typename ...Ts>
    struct TypeList {
        static constexpr int size = sizeof ...(Ts);

        using type = TypeList;

        struct IsTypeList {};

        template<typename ...T> using append = TypeList<Ts..., T...>;

        template<typename ...T> using prepend = TypeList<T..., Ts...>;

        template<template<typename...> typename T> using to = T<Ts...>;
    };

    template<typename TypeList>
    concept TL = requires {
        typename TypeList::IsTypeList;
    };

    template<TL In, template<typename> typename F>
    struct Map;

    template<template<typename> typename F, typename ...Ts>
    struct Map<TypeList<Ts...>, F>: TypeList<typename F<Ts>::type...> {};

    template<TL In, template<typename> typename F>
    using Map_t = typename Map<In, F>::type;

    template <typename List, typename T>
    struct Append;

    template <typename... Ts, typename T>
    struct Append<TypeList<Ts...>, T> {
        typedef TypeList<Ts..., T> type;
    };

    template <class Tag>
    TypeList<> GetTypes(Tag*, Rank<0>) { return {}; }

    #define GET_REGISTERED_TYPES(Tag) \
        decltype(GetTypes(static_cast<Tag*>(nullptr), Rank<UIComponentMaxRegisteredTypes>()))

    #define REGISTER_TYPE(Tag, Type) \
        inline Append<GET_REGISTERED_TYPES(Tag), Type>::type GetTypes(Tag*, Rank<GET_REGISTERED_TYPES(Tag)::size + 1>) { \
            return {};                                                                                                   \
        }
}