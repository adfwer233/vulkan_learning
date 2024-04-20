#pragma once

#include <array>
#include <variant>

class VklScene;

enum {
    UIComponentMaxRegisteredTypes = 15
};

template <int N> struct Rank : Rank<N - 1> {};

template <> struct Rank<0> {};

template <int N, typename... Types> using NthTypeOf = typename std::tuple_element<N, std::tuple<Types...>>::type;

class UIManager;

template <typename... Ts> struct UITypeList {
    static const int size = sizeof...(Ts);

  private:
    std::array<void *, size> componentsData;

    template <typename T, T... indices>
    constexpr void constructorLoop(std::integer_sequence<T, indices...>, VklScene &scene, UIManager &uiManager) {
        ((componentsData[indices] = new NthTypeOf<indices, Ts...>(scene, uiManager)), ...);
    }

    template <typename T, T... indices> constexpr void destructorLoop(std::integer_sequence<T, indices...>) {
        ((delete reinterpret_cast<NthTypeOf<indices, Ts...> *>(componentsData[indices])), ...);
    }

    template <typename T, T... indices> constexpr void renderImguiLoop(std::integer_sequence<T, indices...>) {
        ((reinterpret_cast<NthTypeOf<indices, Ts...> *>(componentsData[indices])->renderImgui()), ...);
    }

  public:
    UITypeList() = default;

    ~UITypeList() {
        constexpr int n = sizeof...(Ts);
        constexpr auto indexSeq = std::make_integer_sequence<int, n>{};
        destructorLoop(indexSeq);
    }

    UITypeList(VklScene &scene, UIManager &uiManager) {
        constexpr int n = sizeof...(Ts);
        constexpr auto indexSeq = std::make_integer_sequence<int, n>{};
        constructorLoop(indexSeq, scene, uiManager);
    }

    void renderImgui() {
        constexpr int n = sizeof...(Ts);
        constexpr auto indexSeq = std::make_integer_sequence<int, n>{};
        renderImguiLoop(indexSeq);
    }
};

template <typename List, typename T> struct Append;

template <typename... Ts, typename T> struct Append<UITypeList<Ts...>, T> {
    typedef UITypeList<Ts..., T> type;
};

template <class Tag> UITypeList<> GetTypes(Tag *, Rank<0>) {
    return {};
}

#define GET_REGISTERED_TYPES(Tag) decltype(GetTypes(static_cast<Tag *>(nullptr), Rank<UIComponentMaxRegisteredTypes>()))

#define REGISTER_TYPE(Tag, Type)                                                                                       \
    inline Append<GET_REGISTERED_TYPES(Tag), Type>::type GetTypes(Tag *, Rank<GET_REGISTERED_TYPES(Tag)::size + 1>) {  \
        return {};                                                                                                     \
    }

struct UIManagerRegisteredTypeTag {};