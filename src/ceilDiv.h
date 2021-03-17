#pragma once
#include <type_traits>
#include <cmath>

using std::is_floating_point;
using std::ceil;




template <class T>
T ceilDiv(T x, T y) {
  if (is_floating_point<T>()) return ceil(x/y);
  else return (x + y-1) / y;
}

template <class T>
__device__ T ceilDivD(T x, T y) {
  return (x + y-1) / y;
}
