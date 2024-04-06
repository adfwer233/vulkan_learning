#pragma once

#include "autodiff/reverse/var.hpp"

template<uint32_t dim>
struct autodiff_vec {
    std::array<autodiff::var, dim> data;

    using value_type = autodiff::var;

    size_t size() { return dim; }

    template <std::floating_point ...Values>
    explicit autodiff_vec(Values... values) {
        static_assert(dim == sizeof...(values) || sizeof...(values) == 0);
        int i = 0;
        ((data[i] = values, i++), ...);
    }

    autodiff_vec() {
        for (auto &item: data)
            item = 0;
    }

    autodiff_vec<dim> operator + (const autodiff_vec<dim> &a);
    autodiff_vec<dim> operator - (const autodiff_vec<dim> &a);
    autodiff_vec<dim> operator * (const autodiff_vec<dim> &a);
    autodiff_vec<dim> operator / (const autodiff_vec<dim> &a);

    autodiff::var operator [] (int idx) {
        return data[idx];
    }
};

struct autodiff_vec2: public autodiff_vec<2> {
    template <std::floating_point ...Values>
    explicit autodiff_vec2(Values... values): autodiff_vec<2>(values...) {}
};

struct autodiff_vec3: public autodiff_vec<3> {
    template <std::floating_point ...Values>
    explicit autodiff_vec3(Values... values): autodiff_vec<3>(values...) {}
};

struct autodiff_vec4: public autodiff_vec<4> {

};

template<uint32_t m, uint32_t n>
struct autodiff_mat {
    std::array<std::array<autodiff::var, n>, m> data;
};

struct autodiff_mat2: autodiff_mat<2, 2> {

};

struct autodiff_mat3: autodiff_mat<3, 3> {

};

namespace GeometryAutodiff {
    template<uint32_t dim>
    autodiff::var dot(const autodiff_vec<dim> &vec1, const autodiff_vec<dim> &vec2);
}

#include "autodiff.hpp.impl"