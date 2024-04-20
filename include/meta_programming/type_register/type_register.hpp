#pragma once
#include "../type_list.hpp"

namespace MetaProgramming {

    enum {
        MaxRegisterTypes = 15
    };

    namespace RegisterDetails {
        template<int N> struct Rank: Rank<N - 1> {};
        template<> struct Rank<0> {};

    }

    template <typename Tag> TypeList<> GetTypes(Tag *, RegisterDetails::Rank<0>) {
        return {};
    }
#define META_GET_REGISTERED_TYPES(Tag) decltype(GetTypes(static_cast<Tag *>(nullptr), MetaProgramming::RegisterDetails::Rank<MetaProgramming::MaxRegisterTypes>()))

#define META_REGISTER_TYPE(Tag, Type)                                                                                       \
    inline META_GET_REGISTERED_TYPES(Tag)::append<Type>::type GetTypes(Tag *, MetaProgramming::RegisterDetails::Rank<META_GET_REGISTERED_TYPES(Tag)::size + 1>) {  \
        return {};                                                                                                     \
    }
}

// register tags
struct RenderableGeometryTag{};