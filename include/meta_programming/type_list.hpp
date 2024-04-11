#pragma once

#include <tuple>

namespace MetaProgramming {

template <typename... Ts> struct TypeList {
    static constexpr int size = sizeof...(Ts);

    using type = TypeList;

    struct IsTypeList {};

    template <typename... T> using append = TypeList<Ts..., T...>;

    template <typename... T> using prepend = TypeList<T..., Ts...>;

    template <template <typename...> typename T> using to = T<Ts...>;
};

namespace TypeListFunctions {

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

} // namespace TypeListFunctions
} // namespace MetaProgramming