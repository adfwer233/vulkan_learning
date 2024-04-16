project(geometry VERSION 1.0 DESCRIPTION "Geometry Kernel" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(SRC_FILE
        autodiff/autodiff.cpp
        surface/tensor_product_bezier.cpp
        function/bernstein.cpp
)

add_library(${PROJECT_NAME} ${SRC_FILE})

target_link_libraries(${PROJECT_NAME} glm autodiff eigen vkl Boost::math)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_include_directories(${PROJECT_NAME}
        PUBLIC ${Boost_INCLUDE_DIRS}
)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})