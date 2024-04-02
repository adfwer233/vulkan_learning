#include "gtest/gtest.h"

#include "vkl/templates/vkl_type_list.hpp"

template<typename T>
struct add_pointer {
    using type = T*;
};

using namespace VklTypeList;
using LongList = TypeList<char, float, double, int, char>;
using AList = TypeList<int, float>;

template<typename T>
using SizeLess4 = std::bool_constant<sizeof(T) < 4>;

TEST(VklTypeListTest, MapTest) {
    static_assert(std::is_same_v<Map<LongList, add_pointer>::type, TypeList<char*, float*, double*, int*, char*>>);
    static_assert(std::is_same_v<Map_t<LongList, add_pointer>, TypeList<char*, float*, double*, int*, char*>>);
}

TEST(VklTypeListTest, ToTest) {
    static_assert(std::is_same_v<AList::to<std::tuple>, std::tuple<int, float>>);
    static_assert(std::is_same_v<AList::to<std::variant>, std::variant<int, float>>);
}

TEST(VklTypeListTest, FilterTest) {
    static_assert(std::is_same_v<Filter_t<LongList, SizeLess4>, TypeList<char, char>>);
}

template<typename ACC, typename E>
using TypeSizeAcc = std::integral_constant<size_t, ACC::value + sizeof(E)>;

TEST(VklTypeListTest, FoldTest) {
    static_assert(Fold_t<LongList, std::integral_constant<size_t, 0>, TypeSizeAcc>::value == 18);
}