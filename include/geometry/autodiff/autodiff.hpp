#include "autodiff/reverse/var.hpp"

template<uint32_t dim>
struct autodiff_vec {
    std::array<autodiff::var, dim> data;
};

struct autodiff_vec2: autodiff_vec<2> {

};

struct autodiff_vec3: autodiff_vec<3> {

};

struct autodiff_vec4: autodiff_vec<4> {

};

template<uint32_t m, uint32_t n>
struct autodiff_mat {
    std::array<std::array<autodiff::var, n>, m> data;
};

struct autodiff_mat2: autodiff_mat<2, 2> {

};

struct autodiff_mat3: autodiff_mat<3, 3> {

};