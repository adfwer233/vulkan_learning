#include "gtest/gtest.h"

#include "vkl/templates/vkl_type_list.hpp"

template<typename T>
struct add_pointer {
    using type = T*;
};

TEST(VklTypeListTest, MapTest) {
    using namespace VklTypeList;
    using LongList = TypeList<char, float, double, int, char>;
    static_assert(std::is_same_v<Map<LongList, add_pointer>::type, TypeList<char*, float*, double*, int*, char*>>);
    static_assert(std::is_same_v<Map_t<LongList, add_pointer>, TypeList<char*, float*, double*, int*, char*>>);
}

TEST(VklTypeListTest, ToTest) {
    using namespace VklTypeList;
    using AList = TypeList<int, float>;
    static_assert(std::is_same_v<AList::to<std::tuple>, std::tuple<int, float>>);
    static_assert(std::is_same_v<AList::to<std::variant>, std::variant<int, float>>);
}
