#pragma once
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <utility>
#include <memory>
#include <omp.h>
#include "_cuda.h"
#include "DiGraph.h"
#include "ceilDiv.h"
#include "measure.h"
#include "count.h"
#include "add.h"
#include "sum.h"
#include "fill.h"
#include "multiply.h"
#include "error.h"
#include "vertices.h"
#include "sourceOffsets.h"
#include "destinationIndices.h"
#include "vertexData.h"
#include "vertexContainer.h"

using std::vector;
using std::unordered_map;
using std::unique_ptr;
using std::lower_bound;
using std::swap;
using std::max;
using std::abs;



enum struct PageRankMode {
  BLOCK,
  THREAD,
  DYNAMIC,
  SWITCHED
};

struct PageRankOptimizations {
  bool components;
  bool sort;
  bool identical;
  bool chain;
  bool converged;
};

template <class T>
struct PageRankOptions {
  typedef PageRankMode Mode;
  typedef PageRankOptimizations Optimizations;
  Mode mode;
  Optimizations optimizations;

  T damping;
  T convergence;

  PageRankOptions(Mode _mode=Mode::BLOCK, Optimizations _optimizations={}, T _damping=0.85, T _convergence=1e-6) {
    mode = _mode;
    optimizations = _optimizations;
    damping = _damping;
    convergence = _convergence;
  }
};




template <class G, class C, class T>
T pageRankTeleport(C& r, G& x, T p, int N) {
  T a = (1-p)/N;
  for (auto u : x.vertices())
    if (x.vertexData(u) == 0) a += p*r[u]/N;
  return a;
}

template <class G, class C, class T>
void pageRankFactor(C& a, G& x, T p) {
  int N = x.order();
  for (auto u : x.vertices()) {
    int d = x.vertexData(u);
    a[u] = d>0? p/d : 0;
  }
}


template <class G, class C, class T>
void pageRankStep(C& a, C& c, G& x, T c0) {
  for (auto v : x.vertices())
    a[v] = c0 + sumAt(c, x.edges(v));
}


template <class G, class C, class T>
auto& pageRankLoop(C& a, C& r, C& f, C& c, G& x, T p, T E) {
  T e0 = T();
  int N = x.order();
  fillAt(r, x.vertices(), T(1)/N);
  pageRankFactor(f, x, p);
  while (1) {
    T c0 = pageRankTeleport(r, x, p, N);
    multiply(c, r, f);
    pageRankStep(a, c, x, c0);
    T e1 = absError(a, r);
    if (e1 < E || e1 == e0) break;
    swap(a, r);
    e0 = e1;
  }
  return a;
}

template <class G, class C, class T>
auto& pageRankCore(C& a, C& r, C& f, C& c, G& x, T p, T E) {
  int N = x.order();
  fillAt(r, x.vertices(), T(1)/N);
  pageRankFactor(f, x, p);
  return pageRankLoop(a, r, f, c, x, p, E);
  // fillAt(b, x.nonVertices(), T());
}


template <class G, class H, class T=float>
auto pageRank(float& t, G& x, H& xt, PageRankOptions<T> o=PageRankOptions<T>()) {
  auto p = o.damping;
  auto E = o.convergence;
  auto a = xt.vertexContainer(T());
  auto r = xt.vertexContainer(T());
  auto f = xt.vertexContainer(T());
  auto c = xt.vertexContainer(T());
  t = measureDuration([&]() { pageRankCore(a, r, f, c, xt, p, E); });
  return a;
}




template <class T, class V>
__global__ void pageRankFactorKernel(T *a, V *vdata, T p, int N) {
  DEFINE(t, b, B, G);
  for (int i=B*b+t, DI=G*B; i<N; i+=DI) {
    V d = vdata[i];
    a[i] = d>0? p/d : 0;
  }
}


template <class T>
__device__ bool pageRankDoneKernel(T *a, int v, T E) {
  return E && a[v] < 0;
}

template <class T>
__device__ void pageRankSetKernel(T *a, int v, T E, T r) {
  a[v]  = abs(a[v] - r) < E? -r : r;
}


template <class T>
__global__ void pageRankBlockKernel(T *a, T *c, int *vfrom, int *efrom, T c0, int N, T E) {
  DEFINE(t, b, B, G);
  __shared__ T cache[BLOCK_DIM];

  for (int v=b; v<N; v+=G) {
    if (pageRankDoneKernel(a, E, v)) continue;
    int ebgn = vfrom[v];
    int ideg = vfrom[v+1]-vfrom[v];
    cache[t] = sumAtKernelLoop(c, efrom+ebgn, ideg, t, B);
    sumKernelReduce(cache, B, t);
    if (t == 0) pageRankSetKernel(a, E, v, c0 + cache[0]);
  }
}


template <class T>
__global__ void pageRankThreadKernel(T *a, T *c, int *vfrom, int *efrom, T c0, int N, T E) {
  DEFINE(t, b, B, G);

  for (int v=B*b+t; v<N; v+=G*B) {
    if (pageRankDoneKernel(a, E, v)) continue;
    int ebgn = vfrom[v];
    int ideg = vfrom[v+1]-vfrom[v];
    pageRankSetKernel(a, E, v, c0 + sumAtKernelLoop(c, efrom+ebgn, ideg, 0, 1));
  }
}


template <class T>
__global__ void pageRankDynamicKernel(T *a, T *c, int *vfrom, int *efrom, T c0, int N, T E) {
  DEFINE(t, b, B, G);

  for (int v=B*b+t; v<N; v+=G*B) {
    if (pageRankDoneKernel(a, E, v)) continue;
    int ebgn = vfrom[v];
    int ideg = vfrom[v+1]-vfrom[v];
    if (ideg >= B/2) pageRankBlockKernel<<<1, B>>>(&a[v], c, &vfrom[v], efrom, c0, 1, E);
    else pageRankSetKernel(a, E, v, c0 + sumAtKernelLoop(c, efrom+ebgn, ideg, 0, 1));
  }
}


template <class T>
void pageRankKernelCall(int G, int B, T *a, T *c, int *vfrom, int *efrom, T c0, int N, T E, PageRankMode M, int S) {
  typedef PageRankMode Mode;
  switch (M) {
    default:
    case Mode::BLOCK:   pageRankBlockKernel <<<G, B>>>(a, c, vfrom, efrom, c0, N, E); break;
    case Mode::THREAD:  pageRankThreadKernel<<<G, B>>>(a, c, vfrom, efrom, c0, N, E); break;
    // case Mode::DYNAMIC: pageRankDynamicKernel<<<G, B>>>(a, c, vfrom, efrom, c0, N, E); break;
    case Mode::SWITCHED:
      pageRankThreadKernel<<<G, B>>>(a,   c, vfrom,   efrom, c0, S,   E);
      pageRankBlockKernel <<<G, B>>>(a+S, c, vfrom+S, efrom, c0, N-S, E);
      break;
  }
}

template <class T>
void pageRankKernelCallStreamed(int G, int B, cudaStream_t s1, cudaStream_t s2, T *a, T *c, int *vfrom, int *efrom, T c0, int N, T E, PageRankMode M, int S) {
  typedef PageRankMode Mode;
  switch (M) {
    default:
    case Mode::BLOCK:   pageRankBlockKernel <<<G, B, 0, s1>>>(a, c, vfrom, efrom, c0, N, E); break;
    case Mode::THREAD:  pageRankThreadKernel<<<G, B, 0, s1>>>(a, c, vfrom, efrom, c0, N, E); break;
    // case Mode::DYNAMIC: pageRankDynamicKernel<<<G, B, 0, s1>>>(a, c, vfrom, efrom, c0, N, E); break;
    case Mode::SWITCHED:
      pageRankThreadKernel<<<G, B, 0, s1>>>(a,   c, vfrom,   efrom, c0, S,   E);
      pageRankBlockKernel <<<G, B, 0, s2>>>(a+S, c, vfrom+S, efrom, c0, N-S, E);
      break;
  }
}




template <class T>
T* pageRankCudaLoop(int G, int B, T* e, T *r0, T *eD, T *r0D, T *aD, T *fD, T *rD, T *cD, int *vfromD, int *efromD, int *vdataD, int N, int i, int n, PageRankMode M, T p, T E, int S, bool OD) {
  T e0 = T();
  int G1 = G * sizeof(T);
  while (1) {
    sumIfNotKernel<<<G, B>>>(r0D,  rD,   vdataD, N);
    multiplyKernel<<<G, B>>>(cD+i, rD+i, fD+i,   n);
    TRY( cudaMemcpy(r0, r0D, G1, cudaMemcpyDeviceToHost) );
    T c0 = (1-p)/N + p*sum(r0, G)/N;
    pageRankKernelCall(G, B, aD+i, cD, vconvD+i, vfromD+i, efromD, c0, n, E, M, S);
    errorAbsKernel<<<G, B>>>(eD, rD+i, aD+i, n);
    TRY( cudaMemcpy(e, eD, G1, cudaMemcpyDeviceToHost) );
    T e1 = sum(e, G);
    if (e1 < E || e1 == e0) break;
    swap(aD, rD);
    e0 = e1;
  }
  return aD;
}

template <class T>
T* pageRankCudaCore(T* e, T *r0, T *eD, T *r0D, T *aD, T *fD, T *rD, T *cD, int *vconvD, int *vfromD, int *efromD, int *vdataD, int N, PageRankMode M, T p, T E, int S) {
  int B = BLOCK_DIM;
  int G = min(ceilDiv(N, B), GRID_DIM);
  fillKernel<<<G, B>>>(rD, N, T(1)/N);
  pageRankFactorKernel<<<G, B>>>(fD, vdataD, p, N);
  return pageRankCudaLoop(G, B, e, r0, eD, r0D, aD, fD, rD, cD, vconvD, vfromD, efromD, vdataD, N, 0, N, M, p, E, S);
}




template <class T>
T* pageRankCudaLoopStreamed(int G, int B, cudaStream_t s1, cudaStream_t s2, cudaStream_t s3, T *e, T *r0, T *eD, T *r0D, T *aD, T *fD, T *rD, T *cD, int *vconvD, int *vfromD, int *efromD, int *vdataD, int N, int i, PageRankMode M, T p, T E, int S) {
  T e0 = T();
  int G1 = G * sizeof(T);
  sumIfNotKernel<<<G, B, 0, s1>>>(r0D,  rD+i, vdataD+i, N);
  multiplyKernel<<<G, B, 0, s2>>>(cD+i, rD+i, fD+i, N);
  while (1) {
    TRY( cudaStreamSynchronize(s1) );
    TRY( cudaStreamSynchronize(s2) );
    T c0 = (1-p)/N + p*sum(r0, G)/N;

    pageRankKernelCallStreamed(G, B, s1, s2, aD+i, cD, vconvD+i, vfromD+i, efromD, c0, N, M, S);
    TRY( cudaStreamSynchronize(s1) );
    TRY( cudaStreamSynchronize(s2) );
    swap(aD, rD);

    errorAbsKernel<<<G, B, 0, s3>>>(eD,   rD+i, aD+i, N);
    sumIfNotKernel<<<G, B, 0, s1>>>(r0D,  rD+i, vdataD+i, N);
    multiplyKernel<<<G, B, 0, s2>>>(cD+i, rD+i, fD+i, N);
    TRY( cudaMemcpyAsync(e,  eD,  G1, cudaMemcpyDeviceToHost, s3) );
    TRY( cudaMemcpyAsync(r0, r0D, G1, cudaMemcpyDeviceToHost, s1) );
    TRY( cudaStreamSynchronize(s3) );
    T e1 = sum(e, G);
    if (e1 < E || e1 == e0) break;
    e0 = e1;
  }
  return rD;
}

template <class T>
T* pageRankCudaCoreStreamed(cudaStream_t s1, cudaStream_t s2, cudaStream_t s3, T *e, T *r0, T *eD, T *r0D, T *aD, T *fD, T *rD, T *cD, int *vfromD, int *efromD, int *vdataD, int N, PageRankMode M, T p, T E, int S) {
  int B = BLOCK_DIM;
  int G = min(ceilDiv(N, B), GRID_DIM);
  pageRankFactorKernel<<<G, B, 0, s1>>>(fD, vdataD, p, N);
  fillKernel<<<G, B, 0, s2>>>(rD, N, T(1)/N);
  TRY( cudaStreamSynchronize(s1) );
  TRY( cudaStreamSynchronize(s2) );
  return pageRankCudaLoopStreamed(G, B, s1, s2, s3, e, r0, eD, r0D, aD, fD, rD, cD, vfromD, efromD, vdataD, N, 0, M, p, E, S);
}




template <class G>
auto pageRankVertices(G& x, PageRankMode M) {
  using K = typename G::TKey;
  typedef PageRankMode Mode;
  if (M != Mode::SWITCHED) return vertices(x);
  return verticesBy(x, [&](K u) { return x.degree(u); });
}

template <class G, class K>
int pageRankSwitchPoint(G& x, vector<K>& ks, PageRankMode M) {
  typedef PageRankMode Mode;
  if (M != Mode::SWITCHED) return 0;
  int deg = int(0.5 * BLOCK_DIM);
  auto it = lower_bound(ks.begin(), ks.end(), deg, [&](K u, int d) {
    return x.degree(u) < d;
  });
  return it - ks.begin();
}


template <class G, class H, class T=float>
auto pageRankCuda(float& t, G& x, H& xt, PageRankOptions<T> o=PageRankOptions<T>()) {
  using K = typename G::TKey;
  auto M = o.mode;
  auto p = o.damping;
  auto E = o.convergence;
  auto ks = pageRankVertices(xt, M);
  auto vfrom = sourceOffsets(xt, ks);
  auto efrom = destinationIndices(xt, ks);
  auto vdata = vertexData(xt, ks);  // outDegree
  int S = pageRankSwitchPoint(xt, ks, M);
  int N = xt.order();
  int B = BLOCK_DIM;
  int g = min(ceilDiv(N, B), GRID_DIM);
  int VFROM1 = vfrom.size() * sizeof(int);
  int EFROM1 = efrom.size() * sizeof(int);
  int VDATA1 = vdata.size() * sizeof(int);
  int G1 = g * sizeof(T);
  int N1 = N * sizeof(T);
  vector<T> a(N);

  T *e,  *r0;
  T *eD, *r0D, *fD, *rD, *cD, *aD, *bD;
  int *vfromD, *efromD, *vdataD;
  cudaStream_t s1, s2, s3;
  TRY( cudaProfilerStart() );
  TRY( cudaStreamCreate(&s1) );
  TRY( cudaStreamCreate(&s2) );
  TRY( cudaStreamCreate(&s3) );
  TRY( cudaSetDeviceFlags(cudaDeviceMapHost) );
  TRY( cudaHostAlloc(&e,  G1, cudaHostAllocDefault) );
  TRY( cudaHostAlloc(&r0, G1, cudaHostAllocDefault) );
  TRY( cudaMalloc(&eD,  G1) );
  TRY( cudaMalloc(&r0D, G1) );
  TRY( cudaMalloc(&fD, N1) );
  TRY( cudaMalloc(&rD, N1) );
  TRY( cudaMalloc(&cD, N1) );
  TRY( cudaMalloc(&aD, N1) );
  TRY( cudaMalloc(&vfromD, VFROM1) );
  TRY( cudaMalloc(&efromD, EFROM1) );
  TRY( cudaMalloc(&vdataD, VDATA1) );
  TRY( cudaMemcpyAsync(vfromD, vfrom.data(), VFROM1, cudaMemcpyHostToDevice, s1) );
  TRY( cudaMemcpyAsync(efromD, efrom.data(), EFROM1, cudaMemcpyHostToDevice, s1) );
  TRY( cudaMemcpyAsync(vdataD, vdata.data(), VDATA1, cudaMemcpyHostToDevice, s1) );
  TRY( cudaStreamSynchronize(s1) );

  t = measureDuration([&]() { bD = pageRankCudaCore(e, r0, eD, r0D, aD, fD, rD, cD, vfromD, efromD, vdataD, N, M, p, E, S); });
  TRY( cudaMemcpy(a.data(), bD, N1, cudaMemcpyDeviceToHost) );

  TRY( cudaFree(eD) );
  TRY( cudaFree(r0D) );
  TRY( cudaFree(fD) );
  TRY( cudaFree(rD) );
  TRY( cudaFree(cD) );
  TRY( cudaFree(aD) );
  TRY( cudaFree(vfromD) );
  TRY( cudaFree(efromD) );
  TRY( cudaFree(vdataD) );
  TRY( cudaFreeHost(e) );
  TRY( cudaFreeHost(r0) );
  TRY( cudaStreamDestroy(s1) );
  TRY( cudaStreamDestroy(s2) );
  TRY( cudaStreamDestroy(s3) );
  TRY( cudaProfilerStop() );
  return vertexContainer(xt, a, ks);
}




template <class G, class H, class T=float>
auto pageRankCudaStreamed(float& t, G& x, H& xt, PageRankOptions<T> o=PageRankOptions<T>()) {
  using K = typename G::TKey;
  auto M = o.mode;
  auto p = o.damping;
  auto E = o.convergence;
  auto ks = pageRankVertices(xt, M);
  auto vfrom = sourceOffsets(xt, ks);
  auto efrom = destinationIndices(xt, ks);
  auto vdata = vertexData(xt, ks);  // outDegree
  int S = pageRankSwitchPoint(xt, ks, M);
  int N = xt.order();
  int B = BLOCK_DIM;
  int g = min(ceilDiv(N, B), GRID_DIM);
  int VFROM1 = vfrom.size() * sizeof(int);
  int EFROM1 = efrom.size() * sizeof(int);
  int VDATA1 = vdata.size() * sizeof(int);
  int G1 = g * sizeof(T);
  int N1 = N * sizeof(T);
  vector<T> a(N);

  T *e,  *r0;
  T *eD, *r0D, *fD, *rD, *cD, *aD, *bD;
  int *vfromD, *efromD, *vdataD;
  cudaStream_t s1, s2, s3;
  TRY( cudaProfilerStart() );
  TRY( cudaStreamCreate(&s1) );
  TRY( cudaStreamCreate(&s2) );
  TRY( cudaStreamCreate(&s3) );
  TRY( cudaSetDeviceFlags(cudaDeviceMapHost) );
  TRY( cudaHostAlloc(&e,  G1, cudaHostAllocDefault) );
  TRY( cudaHostAlloc(&r0, G1, cudaHostAllocDefault) );
  TRY( cudaMalloc(&eD,  G1) );
  TRY( cudaMalloc(&r0D, G1) );
  TRY( cudaMalloc(&fD, N1) );
  TRY( cudaMalloc(&rD, N1) );
  TRY( cudaMalloc(&cD, N1) );
  TRY( cudaMalloc(&aD, N1) );
  TRY( cudaMalloc(&vfromD, VFROM1) );
  TRY( cudaMalloc(&efromD, EFROM1) );
  TRY( cudaMalloc(&vdataD, VDATA1) );
  TRY( cudaMemcpyAsync(vfromD, vfrom.data(), VFROM1, cudaMemcpyHostToDevice, s1) );
  TRY( cudaMemcpyAsync(efromD, efrom.data(), EFROM1, cudaMemcpyHostToDevice, s1) );
  TRY( cudaMemcpyAsync(vdataD, vdata.data(), VDATA1, cudaMemcpyHostToDevice, s1) );
  TRY( cudaStreamSynchronize(s1) );

  t = measureDuration([&]() { bD = pageRankCudaCoreStreamed(s1, s2, s3, e, r0, eD, r0D, aD, fD, rD, cD, vfromD, efromD, vdataD, N, M, p, E, S); });
  TRY( cudaMemcpyAsync(a.data(), bD, N1, cudaMemcpyDeviceToHost, s3) );
  TRY( cudaStreamSynchronize(s3) );

  TRY( cudaFree(eD) );
  TRY( cudaFree(r0D) );
  TRY( cudaFree(fD) );
  TRY( cudaFree(rD) );
  TRY( cudaFree(cD) );
  TRY( cudaFree(aD) );
  TRY( cudaFree(vfromD) );
  TRY( cudaFree(efromD) );
  TRY( cudaFree(vdataD) );
  TRY( cudaFreeHost(e) );
  TRY( cudaFreeHost(r0) );
  TRY( cudaStreamDestroy(s1) );
  TRY( cudaStreamDestroy(s2) );
  TRY( cudaStreamDestroy(s3) );
  TRY( cudaProfilerStop() );
  return vertexContainer(xt, a, ks);
}
