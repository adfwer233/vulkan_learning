#pragma once

#include <tuple>

namespace VklTypeList {

enum {
    MaxRegisteredTypes = 15
};

template <int N> struct Rank : Rank<N - 1> {};

template <> struct Rank<0> {};

template <int N, typename... Types> using NthTypeOf = typename std::tuple_element<N, std::tuple<Types...>>::type;

template <typename... Ts> struct TypeList {
    static constexpr int size = sizeof...(Ts);

    using type = TypeList;

    struct IsTypeList {};

    template <typename... T> using append = TypeList<Ts..., T...>;

    template <typename... T> using prepend = TypeList<T..., Ts...>;

    template <template <typename...> typename T> using to = T<Ts...>;
};

template <typename TypeList>
concept TL = requires { typename TypeList::IsTypeList; };

// Map

template <TL In, template <typename> typename F> struct Map;

template <template <typename> typename F, typename... Ts>
struct Map<TypeList<Ts...>, F> : TypeList<typename F<Ts>::type...> {};

template <TL In, template <typename> typename F> using Map_t = typename Map<In, F>::type;

// Filter

template <TL In, template <typename> typename P, TL Out> struct Filter : Out {};

template <template <typename> class P, TL Out, typename H, typename... Ts>
struct Filter<TypeList<H, Ts...>, P, Out>
    : std::conditional_t<P<H>::value, Filter<TypeList<Ts...>, P, typename Out::template append<H>>,
                         Filter<TypeList<Ts...>, P, Out>> {};

template <TL In, template <typename> typename P> using Filter_t = typename Filter<In, P, TypeList<>>::type;

// Fold

template <typename T> struct Return {
    using type = T;
};

template <TL In, typename Init, template <typename, typename> typename Op> struct Fold : Return<Init> {};

template <typename ACC, template <typename, typename> class Op, typename H, typename... Ts>
struct Fold<TypeList<H, Ts...>, ACC, Op> : Fold<TypeList<Ts...>, typename Op<ACC, H>::type, Op> {};

template <TL In, typename Init, template <typename, typename> class Op> using Fold_t = Fold<In, Init, Op>::type;

template <typename List, typename T> struct Append;

template <typename... Ts, typename T> struct Append<TypeList<Ts...>, T> {
    typedef TypeList<Ts..., T> type;
};

template <class Tag> TypeList<> GetTypes(Tag *, Rank<0>) {
    return {};
}

#define GET_REGISTERED_TYPES(Tag) decltype(GetTypes(static_cast<Tag *>(nullptr), Rank<UIComponentMaxRegisteredTypes>()))

#define REGISTER_TYPE(Tag, Type)                                                                                       \
    inline Append<GET_REGISTERED_TYPES(Tag), Type>::type GetTypes(Tag *, Rank<GET_REGISTERED_TYPES(Tag)::size + 1>) {  \
        return {};                                                                                                     \
    }
} // namespace VklTypeList