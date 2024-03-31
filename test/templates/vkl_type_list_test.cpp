#include "gtest/gtest.h"

#include "vkl/templates/vkl_type_list.hpp"

TEST(VklTypeListTest, MapTest) {
    using namespace VklTypeList;
    using LongList = TypeList<char, float, double, int, char>;
//    EXPECT_EQ(std::is_same_v<Map>)
}
