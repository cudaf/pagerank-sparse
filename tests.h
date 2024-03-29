#pragma once
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <stdio.h>
#include "src/main.h"
#include "setups.h"

using namespace std;




const char* testCeilDiv() {
  if (!(
    ceilDiv(0, 5) == 0 &&
    ceilDiv(1, 5) == 1 &&
    ceilDiv(4, 5) == 1 &&
    ceilDiv(5, 5) == 1 &&
    ceilDiv(6, 5) == 2
  )) return "ceilDiv(int)";
  if (!(
    ceilDiv(0.0f, 0.5f) == 0.0f &&
    ceilDiv(0.1f, 0.5f) == 1.0f &&
    ceilDiv(0.4f, 0.5f) == 1.0f &&
    ceilDiv(0.5f, 0.5f) == 1.0f &&
    ceilDiv(0.6f, 0.5f) == 2.0f
  )) return "ceilDiv(float)";
  if (!(
    ceilDiv(0.0, 0.5) == 0.0 &&
    ceilDiv(0.1, 0.5) == 1.0 &&
    ceilDiv(0.4, 0.5) == 1.0 &&
    ceilDiv(0.5, 0.5) == 1.0 &&
    ceilDiv(0.6, 0.5) == 2.0
  )) return "ceilDiv(double)";
  return NULL;
}


const char* testFrom() {
  vector<vector<int>> x {{1, 2, 3}, {4, 5, 6}};
  auto p = setFrom(x, 1);
  if (!(
    p.count(1) == 0 &&
    p.count(4) == 0 &&
    p.count(2) == 1 &&
    p.count(3) == 1 &&
    p.count(5) == 1 &&
    p.count(6) == 1
  )) return "setFrom";
  return NULL;
}


const char* testErase() {
  vector<int> a {0, 1, 2, 3};
  vector<int> b {0, 1, 2, 3};
  vector<int> c {0, 1, 2, 3};
  vector<int> d {0, 1, 2, 3};
  vector<int> e {0, 1, 2, 3};
  vector<int> f {0, 1, 2, 3};
  eraseAt(a, 0);    // 123
  eraseAt(b, 0, 2); // 23
  eraseAt(c, 1);    // 023
  eraseAt(d, 1, 2); // 03
  eraseAt(e, 3);    // 012
  eraseAt(f, 2, 2); // 01
  if (a[0] != 1 || a.size() != 3) return "eraseAtFront1";
  if (b[0] != 2 || b.size() != 2) return "eraseAtFront2";
  if (c[1] != 2 || c.size() != 3) return "eraseAtMid1";
  if (d[1] != 3 || d.size() != 2) return "eraseAtMid2";
  if (e[2] != 2 || e.size() != 3) return "eraseAtBack1";
  if (f[1] != 1 || f.size() != 2) return "eraseAtBack2";
  return NULL;
}


const char* testInsert() {
  vector<int> a {0, 1, 2, 3};
  vector<int> b {0, 1, 2, 3};
  vector<int> c {0, 1, 2, 3};
  vector<int> d {0, 1, 2, 3};
  vector<int> e {0, 1, 2, 3};
  vector<int> f {0, 1, 2, 3};
  insertAt(a, 0, 9);    // 90123
  insertAt(b, 0, 2, 9); // 990123
  insertAt(c, 1, 9);    // 09123
  insertAt(d, 1, 2, 9); // 099123
  insertAt(e, 4, 9);    // 01239
  insertAt(f, 4, 2, 9); // 012399
  if (a[0] != 9 || a.size() != 5) return "insertAtFront1";
  if (b[1] != 9 || b.size() != 6) return "insertAtFront2";
  if (c[1] != 9 || c.size() != 5) return "insertAtMid1";
  if (d[2] != 9 || d.size() != 6) return "insertAtMid2";
  if (e[4] != 9 || e.size() != 5) return "insertAtBack1";
  if (f[5] != 9 || f.size() != 6) return "insertAtBack2";
  return NULL;
}


const char* testFilter() {
  vector<int> a {0, 1, 2, 3, 4, 5, 6, 7};
  auto b = filter(a.begin(), a.end(), [](int v) { return v % 4 != 0; });
  auto c = filter(b.begin(), b.end(), [](int v) { return v % 3 != 0; });
  auto d = filter(c.begin(), c.end(), [](int v) { return v % 2 != 0; });
  auto e = filter(a, [](int v) { return v % 4 != 0; }); // 123567
  auto f = filter(e, [](int v) { return v % 3 != 0; }); // 1257
  auto g = filter(f, [](int v) { return v % 2 != 0; }); // 157
  if (distance(b.begin(), b.end()) != 6) return "filterIBIE1";
  if (distance(c.begin(), c.end()) != 4) return "filterIBIE2";
  if (distance(d.begin(), d.end()) != 3) return "filterIBIE3";
  if (distance(e.begin(), e.end()) != 6) return "filterC1";
  if (distance(f.begin(), f.end()) != 4) return "filterC2";
  if (distance(g.begin(), g.end()) != 3) return "filterC3";
  return NULL;
}


const char* testFind() {
  vector<int> a {0, 1, 2, 3};
  auto i = find(a.begin(), a.end(), 2)-a.begin();
  auto j = find(a.begin(), a.end(), 9)-a.begin();
  auto k = find(a, 2)-a.begin();
  auto l = find(a, 9)-a.begin();
  auto m = findAt(a, 2);
  auto n = findAt(a, 9);
  if (i != 2) return "findIBIE1";
  if (j != 4) return "findIBIE2";
  if (k != 2) return "findC1";
  if (l != 4) return "findC2";
  if (m != 2) return "findAt1";
  if (n != -1) return "findAt2";
  return NULL;
}


const char* testLowerBound() {
  vector<int> a {0, 1, 2, 3};
  auto i = lowerBound(a.begin(), a.end(), 2)-a.begin();
  auto j = lowerBound(a.begin(), a.end(), 9)-a.begin();
  auto k = lowerBound(a, 2)-a.begin();
  auto l = lowerBound(a, 9)-a.begin();
  auto m = lowerBoundAt(a, 2);
  auto n = lowerBoundAt(a, 9);
  if (i != 2) return "lowerBoundIBIE1";
  if (j != 4) return "lowerBoundIBIE2";
  if (k != 2) return "lowerBoundC1";
  if (l != 4) return "lowerBoundC2";
  if (m != 2) return "lowerBoundAt1";
  if (n != 4) return "lowerBoundAt2";
  return NULL;
}


const char* testTransform() {
  vector<int> a {0, 1, 2, 3};
  auto b = transform(a.begin(), a.end(), [](int v) { return v+1; });
  auto c = transform(b.begin(), b.end(), [](int v) { return v+1; });
  auto d = transform(c.begin(), c.end(), [](int v) { return v+1; });
  auto e = transform(a, [](int v) { return v+1; }); // 1234
  auto f = transform(e, [](int v) { return v+1; }); // 2345
  auto g = transform(f, [](int v) { return v+1; }); // 3456
  for (auto&& v : b) if (v<1 || v>4) return "transformIBIE1";
  for (auto&& v : c) if (v<2 || v>5) return "transformIBIE2";
  for (auto&& v : d) if (v<3 || v>6) return "transformIBIE3";
  for (auto&& v : e) if (v<1 || v>4) return "transformC1";
  for (auto&& v : f) if (v<2 || v>5) return "transformC2";
  for (auto&& v : g) if (v<3 || v>6) return "transformC3";
  return NULL;
}


const char* testRange() {
  int n = 0;
  double v = 0, V = 10, DV = 0.5;
  for (double x : range(v, V, DV))
    if (x != v+DV*(n++)) return "range";
  if (n != 20) return "range";
  return NULL;
}


const char* testFill() {
  vector<int> a {1, 2, 3, 4};
  vector<int> b {1, 2, 3, 4};
  vector<int> c {1, 2, 3, 4};
  vector<int> d {1, 2, 3, 4};
  unordered_map<char, int> m {{'a', 1}, {'b', 2}, {'c', 3}};
  unordered_map<char, int> n {{'a', 1}, {'b', 2}, {'c', 3}};
  vector<int> is {0, 2};
  vector<char> ks {'a', 'c'};

  fill(a, 9);
  fillOmp(b, 9);
  fillCuda(c, 9);
  fill(m, 9);
  fillAt(d, 9, is);
  fillAt(n, 9, ks);
  for (auto& v : a) if (v != 9) return "fillV";
  for (auto& v : b) if (v != 9) return "fillOmpV";
  for (auto& v : c) if (v != 9) return "fillCudaV";
  for (auto& p : m) if (p.second != 9) return "fillM";
  if (!(d[0]==9 && d[1]==2 && d[2]==9 && d[3]==4)) return "fillAtV";
  if (!(n['a']==9 && n['b']==2 && n['c']==9)) return "fillAtM";
  return NULL;
}


const char* testAdd() {
  vector<int> a {1, 2, 3, 4};
  vector<int> b {1, 2, 3, 4};
  vector<int> c {1, 2, 3, 4};
  vector<int> d {1, 2, 3, 4};
  unordered_map<char, int> m {{'a', 1}, {'b', 2}, {'c', 3}};
  unordered_map<char, int> n {{'a', 1}, {'b', 2}, {'c', 3}};
  vector<int> is {0, 2};
  vector<char> ks {'a', 'c'};

  addValue(a, 1);
  addValueOmp(b, 1);
  addValueCuda(c, 1);
  addValue(m, 1);
  addValueAt(d, 1, is);
  addValueAt(n, 1, ks);
  if (!(a[0]==2 && a[1]==3 && a[2]==4 && a[3]==5)) return "addV";
  if (!(b[0]==2 && b[1]==3 && b[2]==4 && b[3]==5)) return "addOmpV";
  if (!(c[0]==2 && c[1]==3 && c[2]==4 && c[3]==5)) return "addCudaV";
  if (!(m['a']==2 && m['b']==3 && m['c']==4)) return "addM";
  if (!(d[0]==2 && d[1]==2 && d[2]==4 && d[3]==4)) return "addAtV";
  if (!(n['a']==2 && n['b']==2 && n['c']==4)) return "addAtM";
  return NULL;
}


const char* testSum() {
  vector<int> a {1, 2, 3, 4};
  unordered_map<char, int> m {{'a', 1}, {'b', 2}, {'c', 3}};
  vector<int> is {0, 2};
  vector<char> ks {'a', 'c'};

  int p = sum(a);
  int q = sumOmp(a);
  int r = sumCuda(a);
  int s = sum(m);
  int t = sumAt(a, is);
  int u = sumAt(m, ks);
  if (p != 10) return "sumV";
  if (q != 10) return "sumOmpV";
  if (r != 10) return "sumCudaV";
  if (s != 6) return "sumM";
  if (t != 4) return "sumAtV";
  if (u != 4) return "sumAtM";
  return NULL;
}


const char* testDotProduct() {
  vector<int> x {1, 2, 3, 4};
  vector<int> y {1, 0, 1, 0};

  int a = dotProduct(x, y);
  int b = dotProductCuda(x, y);
  if (a != 4) return "dotProduct";
  if (b != 4) return "dotProductCuda";
  return NULL;
}


const char* testAbsError() {
  vector<int> x {1, 2, 3, 4};
  vector<int> y {1, 1, 3, 5};
  unordered_map<char, int> m {{'a', 1}, {'b', 2}, {'c', 3}};
  unordered_map<char, int> n {{'a', 1}, {'b', 1}, {'c', 4}};

  int a = absError(x, y);
  int b = absErrorCuda(x, y);
  int c = absError(m, n);
  if (a != 2) return "absErrorV";
  if (b != 2) return "absErrorCudaV";
  if (c != 2) return "absErrorM";
  return NULL;
}


const char* testDiGraph() {
  DiGraph<int, int, int> g;
  DiGraph<char, int, int> h;

  g.addVertex(1, 10);
  g.addEdge(1, 2, 12);
  g.addEdge(2, 4, 24);
  g.addEdge(4, 3, 43);
  g.addEdge(3, 1, 31);
  g.removeEdge(1, 2);

  h.addVertex('a', 10);
  h.addEdge('a', 'b', 12);
  h.addEdge('b', 'd', 24);
  h.addEdge('d', 'c', 43);
  h.addEdge('c', 'a', 31);
  h.removeEdge('a', 'b');

  if (!(
    g.span()  == 5  &&
    g.order() == 4  &&
    g.size()  == 3  &&
    g.hasVertex(1)  && g.hasVertex(2)  && g.hasVertex(3)  && g.hasVertex(4) &&
    g.hasEdge(2, 4) && g.hasEdge(4, 3) && g.hasEdge(3, 1)
  )) return "DiGraphIntProp";
  for (int u : g.vertices()) {
    if (!(
      u >= 1 && u <= 4 &&
      g.degree(u) <= 1 &&
      g.vertexData(u) <= 10
    )) return "DiGraphIntVertex";
    for (int v : g.edges(u))
      if (!(
        v >= 1 && v <= 4 &&
        g.edgeData(u, v) == 10*u+v
      )) return "DiGraphIntEdge";
  }

  if (!(
    h.span()  == 4  &&
    h.order() == 4  &&
    h.size()  == 3  &&
    h.hasVertex('a')  && h.hasVertex('b')  && h.hasVertex('c')  && h.hasVertex('d') &&
    h.hasEdge('b', 'd') && h.hasEdge('d', 'c') && h.hasEdge('c', 'a')
  )) return "DiGraphCharProp";
  for (char u : h.vertices()) {
    if (!(
      u >= 'a' && u <= 'd' &&
      h.degree(u) <= 1 &&
      h.vertexData(u) <= 10
    )) return "DiGraphCharVertex";
    for (char v : h.edges(u))
      if (!(
        v >= 'a' && v <= 'd' &&
        h.edgeData(u, v) == 10*(u-'a'+1)+(v-'a'+1)
      )) return "DiGraphCharEdge";
  }
  return NULL;
}


const char* testCopy() {
  DiGraph<> g;
  g.addEdge(1, 2);
  g.addEdge(2, 4);
  g.addEdge(4, 3);
  g.addEdge(3, 1);
  auto h = copy(g);

  if (!(
    h.order() == 4  &&
    h.size()  == 4  &&
    h.hasEdge(1, 2) &&
    h.hasEdge(2, 4) &&
    h.hasEdge(4, 3) &&
    h.hasEdge(3, 1)
  )) return "copy";
  return NULL;
}


const char* testTranspose() {
  DiGraph<> g;
  DiGraph<> h;
  g.addEdge(1, 2);
  g.addEdge(2, 4);
  g.addEdge(4, 3);
  g.addEdge(3, 1);
  transpose(g, h);

  if (!(
   h.order() == 4  &&
   h.size()  == 4  &&
   h.hasEdge(2, 1) &&
   h.hasEdge(4, 2) &&
   h.hasEdge(3, 4) &&
   h.hasEdge(1, 3)
  )) return "transpose";
  return NULL;
}


const char* testDfs() {
  vector<int> a {1, 2, 4, 3, 5, 6, 8, 7};
  vector<int> b {1, 2, 3, 4, 5};
  vector<int> c {1, 2, 3, 5, 4, 6};
  auto& g = MIN2C;
  auto& h = MIN2D;
  auto& i = MINNV;

  auto p = dfs(g, 1);
  auto q = dfs(h, 1);
  auto r = dfs(i, 1);
  if (p != a) return "dfsMin2c";
  if (q != b) return "dfsMin2d";
  if (r != c) return "dfsMinnv";
  return NULL;
}


const char* testComponents() {
  vector<vector<int>> a {{1, 3, 4, 2}, {5, 7, 8, 6}};
  vector<vector<int>> b {{1}, {4}, {5}, {2}, {3}};
  vector<vector<int>> c {{1, 3}, {5, 4, 6}, {2}};
  auto& g = MIN2C;
  auto& h = MIN2D;
  auto& i = MINNV;
  auto gt = transpose(g);
  auto ht = transpose(h);
  auto it = transpose(i);

  transpose(g, gt);
  transpose(h, ht);
  transpose(i, it);

  auto p = components(g, gt);
  auto q = components(h, ht);
  auto r = components(i, it);
  if (p != a) return "componentsMin2c";
  if (q != b) return "componentsMin2d";
  if (r != c) return "componentsMinnv";
  return NULL;
}


const char* testSort() {
  vector<int> a {1, 2, 4, 5, 6, 8, 7, 3};
  vector<int> b {1, 4, 5, 2, 3};
  vector<int> c {1, 3, 5, 4, 6, 2};
  auto& g = MIN2C;
  auto& h = MIN2D;
  auto& i = MINNV;

  auto p = sort(g);
  auto q = sort(h);
  auto r = sort(i);
  if (p != a) return "sortMin2c";
  if (q != b) return "sortMin2d";
  if (r != c) return "sortMinnv";
  return NULL;
}


const char* testIdenticals() {
  vector<vector<int>> a {{4, 5}, {7, 8}};
  DiGraph<> g;
  g.addEdge(1, 4);
  g.addEdge(1, 5);
  g.addEdge(2, 4);
  g.addEdge(2, 5);
  g.addEdge(3, 4);
  g.addEdge(3, 5);
  g.addEdge(3, 6);
  g.addEdge(4, 7);
  g.addEdge(4, 8);
  g.addEdge(5, 7);
  g.addEdge(5, 8);
  g.addEdge(7, 1);
  g.addEdge(8, 3);
  auto gt = transpose(g);

  auto p = inIdenticals(g, gt);
  if (p != a) return "inIdenticals";
  return NULL;
}


const char* testChains() {
  vector<vector<int>> a {{2, 3, 4}, {5, 6, 7}};
  DiGraph<> g;
  g.addEdge(1, 2);
  g.addEdge(1, 5);
  g.addEdge(2, 3);
  g.addEdge(3, 4);
  g.addEdge(5, 6);
  g.addEdge(6, 7);
  g.addEdge(4, 8);
  g.addEdge(7, 8);
  g.addEdge(8, 9);
  g.addEdge(9, 1);
  auto gt = transpose(g);

  auto p = chains(g, gt);
  if (p != a) return "chains";
  return NULL;
}


const char* testPageRank() {
  typedef PageRankMode  Mode;
  typedef PageRankFlags Flags;
  using G = decltype(MIN2C);
  vector<float> a {0,
    0.045040868326612485,
    0.05703473807762061,
    0.030930433092920914,
    0.04298976368298876,
    0.24661630251804945,
    0.1235621720870247,
    0.22523400274373884,
    0.22859171947104429
  };
  vector<float> b {0,
    0.09950567284608347,
    0.12769835091442033,
    0.4088563412945265,
    0.12769835091442033,
    0.2362412840305493
  };
  vector<float> c {0,
    0.012119784428754443, 0.010576796064342956,
    0.019567072719044492, 0.01612080000141513,
    0.011710417143267388, 0.07839244609203286,
    0.07377618658154286,  0.06985312541028553,
    0.04237448488634947,  0.0381820598964617,
    0.03959764267900844,  0.01836220312872412,
    0.03114307555495728,  0.09030497097372668,
    0.08390010864031147,  0.0428002866707024,
    0.044663939075725474, 0.04510756201371743,
    0.04548387516760983,  0.0881253137439066,
    0.09783784912811343
  };
  vector<float> d {0,
    0.02017396061154734,  0.016468670102029986,
    0.016468670102029986, 0.028892291222176396,
    0.023243825168349837, 0.02765198823523682,
    0.01964683184211511,  0.043668184004720674,
    0.0567647882467902,   0.016967005532428924,
    0.0320197718474144,   0.09566851162184489,
    0.04855385428187146,  0.09343984852659853,
    0.02853012491159028,  0.13609421793617826,
    0.08986160238623807,  0.06573656738092894,
    0.1401492860399099
  };
  vector<float> e {0,
    0.05170616341518244,
    0.07368172600710296,
    0.0574140637880373,
    0.3487008310282128,
    0.19990313374680874,
    0.2685940820146559
  };
  auto& g = MIN2C;
  auto& h = MIN2D;
  auto& i = MIN4C;
  auto& j = MIN5C;
  auto& k = MINNV;
  auto gt = transposeWithDegree(g);
  auto ht = transposeWithDegree(h);
  auto it = transposeWithDegree(i);
  auto jt = transposeWithDegree(j);
  auto kt = transposeWithDegree(k);
  float t, R, E = 1e-5;

  auto p1 = pageRankPush(t, R, g, gt);
  auto p2 = pageRank(t, R, g, gt);
  auto q1 = pageRankPush(t, R, h, ht);
  auto q2 = pageRank(t, R, h, ht);
  auto r1 = pageRankPush(t, R, i, it);
  auto r2 = pageRank(t, R, i, it);
  auto s1 = pageRankPush(t, R, j, jt);
  auto s2 = pageRank(t, R, j, jt);
  auto t1 = pageRankPush(t, R, k, kt);
  auto t2 = pageRank(t, R, k, kt);
  if (absError(a, p1) >= E) return "pageRankPushMin2c";
  if (absError(a, p2) >= E) return "pageRankMin2c";
  if (absError(b, q1) >= E) return "pageRankPushMin2d";
  if (absError(b, q2) >= E) return "pageRankMin2d";
  if (absError(c, r1) >= E) return "pageRankPushMin4c";
  if (absError(c, r2) >= E) return "pageRankMin4c";
  if (absError(d, s1) >= E) return "pageRankPushMin5c";
  if (absError(d, s2) >= E) return "pageRankMin5c";
  if (absError(e, t1) >= E) return "pageRankPushMinNv";
  if (absError(e, t2) >= E) return "pageRankMinNv";
  for (int o=0; o<256; o++) {
    Flags F(o);
    if (!isValid(F)) continue;
    auto p3 = pageRankCuda(t, R, g, gt, {Mode::BLOCK, F});
    auto p4 = pageRankCuda(t, R, g, gt, {Mode::THREAD, F});
    auto p5 = pageRankCuda(t, R, g, gt, {Mode::SWITCHED, F});
    auto q3 = pageRankCuda(t, R, h, ht, {Mode::BLOCK, F});
    auto q4 = pageRankCuda(t, R, h, ht, {Mode::THREAD, F});
    auto q5 = pageRankCuda(t, R, h, ht, {Mode::SWITCHED, F});
    auto r3 = pageRankCuda(t, R, i, it, {Mode::BLOCK, F});
    auto r4 = pageRankCuda(t, R, i, it, {Mode::THREAD, F});
    auto r5 = pageRankCuda(t, R, i, it, {Mode::SWITCHED, F});
    auto s3 = pageRankCuda(t, R, j, jt, {Mode::BLOCK, F});
    auto s4 = pageRankCuda(t, R, j, jt, {Mode::THREAD, F});
    auto s5 = pageRankCuda(t, R, j, jt, {Mode::SWITCHED, F});
    auto t3 = pageRankCuda(t, R, k, kt, {Mode::BLOCK, F});
    auto t4 = pageRankCuda(t, R, k, kt, {Mode::THREAD, F});
    auto t5 = pageRankCuda(t, R, k, kt, {Mode::SWITCHED, F});
    if (absError(a, p3) >= E) return "pageRankCudaMin2c {block}";
    if (absError(a, p4) >= E) return "pageRankCudaMin2c {thread}";
    if (absError(a, p5) >= E) return "pageRankCudaMin2c {switched}";
    if (absError(b, q3) >= E) return "pageRankCudaMin2d {block}";
    if (absError(b, q4) >= E) return "pageRankCudaMin2d {thread}";
    if (absError(b, q5) >= E) return "pageRankCudaMin2d {switched}";
    if (absError(c, r3) >= E) return "pageRankCudaMin4c {block}";
    if (absError(c, r4) >= E) return "pageRankCudaMin4c {thread}";
    if (absError(c, r5) >= E) return "pageRankCudaMin4c {switched}";
    if (absError(d, s3) >= E) return "pageRankCudaMin5c {block}";
    if (absError(d, s4) >= E) return "pageRankCudaMin5c {thread}";
    if (absError(d, s5) >= E) return "pageRankCudaMin5c {switched}";
    if (absError(e, t3) >= E) return "pageRankCudaMinNv {block}";
    if (absError(e, t4) >= E) return "pageRankCudaMinNv {thread}";
    if (absError(e, t5) >= E) return "pageRankCudaMinNv {switched}";
  }
  return NULL;
}


void testAll() {
  vector<const char*> ts = {
    testCeilDiv(),
    testFrom(),
    testErase(),
    testInsert(),
    testFilter(),
    testFind(),
    testLowerBound(),
    testRange(),
    testTransform(),
    testFill(),
    testAdd(),
    testSum(),
    testDotProduct(),
    testAbsError(),
    testDiGraph(),
    testCopy(),
    testTranspose(),
    testDfs(),
    testComponents(),
    testSort(),
    testIdenticals(),
    testChains(),
    testPageRank()
  };
  int n = 0;
  for (auto& t : ts) {
    if (!t) continue;
    printf("ERROR: %s() failed!\n", t);
    n++;
  }
  printf("%d/%ld tests failed.\n", n, ts.size());
}
