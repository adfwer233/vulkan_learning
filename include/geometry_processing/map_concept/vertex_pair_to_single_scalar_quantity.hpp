#pragma once

template <typename T>
concept VertexPairToSingleScalarQuantityMap = requires(T t) { t.perform(); };