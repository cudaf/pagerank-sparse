#pragma once
#include <vector>
#include <algorithm>
#include <memory>
#include <omp.h>
#include "_cuda.h"
#include "ceilDiv.h"
#include "sum.h"

using std::vector;
using std::unique_ptr;
using std::max;




// Finds sum of element-by-element product of 2 vectors.
template <class T>
T dotProduct(T *x, T *y, int N) {
  T a = T();
  for (int i=0; i<N; i++)
    a += x[i] * y[i];
  return a;
}

template <class T>
T dotProduct(vector<T>& x, vector<T>& y) {
  return dotProduct(x.data(), y.data(), x.size());
}




template <class T>
T dotProductOmp(T *x, T *y, int N) {
  T a = T();
  #pragma omp parallel for reduction (+:a)
  for (int i=0; i<N; i++)
    a += x[i] * y[i];
  return a;
}

template <class T>
T dotProductOmp(vector<T>& x, vector<T>& y) {
  return dotProductOmp(x.data(), y.data(), x.size());
}




template <class T>
__device__ T dotProductKernelLoop(T *x, T *y, int N, int i, int DI) {
  T a = T();
  for (; i<N; i+=DI)
    a += x[i] * y[i];
  return a;
}

template <class T>
__device__ T dotProductAtKernelLoop(T *x, T *y, int *is, int N, int i, int DI) {
  T a = T();
  for (; i<N; i+=DI)
    a += x[is[i]] * y[is[i]];
  return a;
}


template <class T>
__global__ void dotProductKernel(T *a, T *x, T *y, int N) {
  DEFINE(t, b, B, G);
  __shared__ T cache[BLOCK_DIM];

  cache[t] = dotProductKernelLoop(x, y, N, B*b+t, G*B);
  sumKernelReduce(cache, B, t);
  if (t == 0) a[b] = cache[0];
}


template <class T>
T dotProductCuda(T *x, T *y, int N) {
  int B = BLOCK_DIM;
  int G = min(ceilDiv(N, B), GRID_DIM);
  size_t N1 = N * sizeof(T);
  size_t G1 = G * sizeof(T);
  unique_ptr<T> a(new T[G1]);

  T *xD, *yD, *aD;
  TRY( cudaMalloc(&xD, N1) );
  TRY( cudaMalloc(&yD, N1) );
  TRY( cudaMalloc(&aD, G1) );
  TRY( cudaMemcpy(xD, x, N1, cudaMemcpyHostToDevice) );
  TRY( cudaMemcpy(yD, y, N1, cudaMemcpyHostToDevice) );

  dotProductKernel<<<G, B>>>(aD, xD, yD, N);
  TRY( cudaMemcpy(a.get(), aD, G1, cudaMemcpyDeviceToHost) );

  TRY( cudaFree(yD) );
  TRY( cudaFree(xD) );
  TRY( cudaFree(aD) );
  return sum(a.get(), G);
}

template <class T>
T dotProductCuda(vector<T>& x, vector<T>& y) {
  return dotProductCuda(x.data(), y.data(), x.size());
}
