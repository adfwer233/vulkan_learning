#pragma once

template<typename T>
concept VklModelLoader = requires(T t) {
    t.load_model();
};