#include "autodiff/forward/dual.hpp"
#include "autodiff/reverse/var.hpp"
#include "gtest/gtest.h"

#include <iostream>

TEST(AutodiffTest, SimpleDualTest) {
    using namespace autodiff;

    auto f = [](autodiff::dual x) -> autodiff::dual {
        return 1 + x + x * x;
    };

    autodiff::dual x = 2.0;

    double dudx = autodiff::derivative(f, autodiff::detail::wrt(x), at(x));

    std::cout << dudx << std::endl;
}

TEST(AutodiffTest, SimpleVarTest) {
    using namespace autodiff;

    auto f = [](autodiff::var x) -> var {
        return 1 + x + x * x;
    };

    var x = 1.0;
    var y = 1.0;
    var u = f(x) * f(y);

    auto [ux, uy] = autodiff::reverse::detail::derivativesx(u, autodiff::reverse::detail::wrt(x, y));

    auto [uxx] = autodiff::reverse::detail::derivatives(ux, autodiff::reverse::detail::wrt(x));

    std::cout << uxx << std::endl;
}
