project(test VERSION 1.0 DESCRIPTION "Test Project" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(SRC_FILE
        templates/vkl_type_list_test.cpp
        autodiff/autodiff_test.cpp
        autodiff/geometry_autodiff_test.cpp
        geometry/function/bernstein_test.cpp
        geometry/surface/tensor_product_bezier_test.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILE})

target_link_libraries(${PROJECT_NAME} glm glfw vkl autodiff geometry GTest::gtest_main)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})