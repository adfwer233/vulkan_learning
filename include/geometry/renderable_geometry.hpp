#include <concepts>

template <typename T>
concept RenderableGeometry = requires(T t) {
    typename T::IsRenderableGeometry;
    typename T::render_type;
    { t.getRenderModel() } -> std::same_as<typename T::render_type>;
};