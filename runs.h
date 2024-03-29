#pragma once
#include "src/main.h"

using namespace std;




void runAdd() {
  int N = 64*1024*1024;
  float *x = new float[N], t;
  vector<int> is1(N);
  for (int i=0; i<N; i++) is1[i] = i;
  auto is2 = range(N);
  auto is3 = filter(is2, [&](int i) { return true; });
  t = measureDuration([&]() { addValue(x, N, 1.0f); });
  printf("[%09.3f ms] addValue(x, N, v)\n", t);
  t = measureDuration([&]() { addValueAt(x, 1.0f, is1); });
  printf("[%09.3f ms] addValueAt(x, <vector> is, v)\n", t);
  t = measureDuration([&]() { addValueAt(x, 1.0f, is2); });
  printf("[%09.3f ms] addValueAt(x, <range>  is, v)\n", t);
  t = measureDuration([&]() { addValueAt(x, 1.0f, is3); });
  printf("[%09.3f ms] addValueAt(x, <filter> is, v)\n", t);
}


void runFill() {
  int N = 64*1024*1024;
  float *x = new float[N], t;
  t = measureDuration([&]() { fill(x, N, 1.0f); });
  printf("[%09.3f ms] fill\n", t);
  t = measureDuration([&]() { fillOmp(x, N, 1.0f); });
  printf("[%09.3f ms] fillOmp\n", t);
  t = measureDuration([&]() { fillCuda(x, N, 1.0f); });
  printf("[%09.3f ms] fillCuda\n", t);
  delete[] x;
}


void runSum() {
  int N = 64*1024*1024;
  float *x = new float[N], t;
  fill(x, N, 1.0f);
  t = measureDuration([&]() { sum(x, N); });
  printf("[%09.3f ms] sum\n", t);
  t = measureDuration([&]() { sumOmp(x, N); });
  printf("[%09.3f ms] sumOmp\n", t);
  t = measureDuration([&]() { sumCuda(x, N); });
  printf("[%09.3f ms] sumCuda\n", t);
  delete[] x;
}


void runAbsError() {
  int N = 64*1024*1024;
  float *x = new float[N];
  float *y = new float[N], t;
  fill(x, N, 1.0f);
  fill(y, N, 2.0f);
  t = measureDuration([&]() { absError(x, y, N); });
  printf("[%09.3f ms] absError\n", t);
  t = measureDuration([&]() { absErrorOmp(x, y, N); });
  printf("[%09.3f ms] absErrorOmp\n", t);
  t = measureDuration([&]() { absErrorCuda(x, y, N); });
  printf("[%09.3f ms] absErrorCuda\n", t);
  delete[] x;
  delete[] y;
}


void runDotProduct() {
  int N = 64*1024*1024;
  float *x = new float[N];
  float *y = new float[N], t;
  fill(x, N, 1.0f);
  fill(y, N, 1.0f);
  t = measureDuration([&]() { dotProduct(x, y, N); });
  printf("[%09.3f ms] dotProduct\n", t);
  t = measureDuration([&]() { dotProductOmp(x, y, N); });
  printf("[%09.3f ms] dotProductOmp\n", t);
  t = measureDuration([&]() { dotProductCuda(x, y, N); });
  printf("[%09.3f ms] dotProductCuda\n", t);
  delete[] x;
  delete[] y;
}
