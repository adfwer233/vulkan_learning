#include "coroutine/generator.hpp"

#include "iostream"
#include "experimental/generator"

#include "gtest/gtest.h"

template<typename T>
struct Tree
{
    T value;
    Tree *left{}, *right{};

    [[nodiscard]] Generator<const T> traverse_inorder() const
    {
        if (left)
            for (const T& x : left->traverse_inorder())
                co_yield x;

        co_yield value;
        if (right)
            for (const T& x : right->traverse_inorder())
                co_yield x;
    }
};

TEST(CoroutineTest, GeneratorTest)
{
    Tree<char> tree[]
            {
                    {'D', tree + 1, tree + 2},
                    //                            │
                    //            ┌───────────────┴────────────────┐
                    //            │                                │
                    {'B', tree + 3, tree + 4},       {'F', tree + 5, tree + 6},
                    //            │                                │
                    //  ┌─────────┴─────────────┐      ┌───────────┴─────────────┐
                    //  │                       │      │                         │
                    {'A'},                  {'C'}, {'E'},                    {'G'}
            };

    for (auto x: tree->traverse_inorder()) {
        std::cout << x << ' ';
    }
    std::cout << std::endl;
}