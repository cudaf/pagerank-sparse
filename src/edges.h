#pragma once
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cstdlib>

using std::vector;
using std::unordered_map;
using std::sort;
using std::rand;
using std::min;




// EDGE
// ----

template <class G, class K>
auto edge(G& x, K u) {
  for (K v : x.edges(u))
    return v;
  return K();
}




// EDGES
// -----

template <class G, class K>
auto edges(G& x, K u) {
  vector<K> a;
  for (auto v : x.edges(u))
    a.push_back(v);
  return a;
}

template <class G, class K>
auto inEdges(G& x, K v) {
  vector<K> a;
  for (auto u : x.inEdges(v))
    a.push_back(u);
  return a;
}




// EDGES-EQUAL
// -----------

template <class G, class K>
bool edgesEqual(G& x, K u1, K u2) {
  auto i1 = x.edges(u1);
  auto i2 = x.edges(u2);
  auto b1 = i1.begin();
  auto b2 = i2.begin();
  auto e1 = i1.end();
  auto e2 = i2.end();
  for (; b1 != e1 && b2 != e2; b1++, b2++)
    if (*b1 != *b2) return false;
  return b1 == e1 && b2 == e2;
}

template <class G, class K>
bool inEdgesEqual(G& x, K u1, K u2) {
  auto i1 = x.edges(u1);
  auto i2 = x.edges(u2);
  auto b1 = i1.begin();
  auto b2 = i2.begin();
  auto e1 = i1.end();
  auto e2 = i2.end();
  for (; b1 != e1 && b2 != e2; b1++, b2++)
    if (*b1 != *b2) return false;
  return b1 == e1 && b2 == e2;
}




// EDGES-DATA
// ----------

template <class G, class C>
auto edgeData(G& x, C&& ks) {
  using E = typename G::TEdge;
  vector<E> a;
  for (auto u : ks) {
    for (auto v : x.edges(u))
      a.push_back(x.edgeData(u, v));
  }
  return a;
}

template <class G>
auto edgeData(G& x) {
  return edgeData(x, x.vertices());
}




// DESTINATION-INDICES
// -------------------

template <class G, class C>
auto destinationIndices(G& x, C&& ks) {
  using K = typename G::TKey;
  vector<int> a;
  unordered_map<K, int> id;
  int i = 0;
  for (auto u : ks)
    id[u] = i++;
  for (auto u : ks) {
    for (auto v : x.edges(u))
      a.push_back(id[v]);
    // sort(a.end()-x.degree(u), a.end());
  }
  return a;
}

template <class G>
auto destinationIndices(G& x) {
  return destinationIndices(x, x.vertices());
}




// REMOVE-RANDOM-EDGE
// ------------------

template <class G, class K>
void removeRandomEdge(G& a, K u) {
  if (a.degree(u) == 0) return;
  K vi = rand() % a.degree(u); int i = 0;
  for (auto v : a.edges(u))
    if (i++ == vi) { a.removeEdge(u, v); break; }
}


template <class G>
void removeRandomEdge(G& a) {
  using K = typename G::TKey;
  K u = rand() % a.span();
  removeRandomEdge(a, u);
}


template <class G>
void removeRandomEdgeByDegree(G& a) {
  int v = rand() % a.size(), n = 0;
  for (auto u : a.vertices()) {
    if (v > n+a.degree(u)) n += a.degree(u);
    else { removeRandomEdge(a, u); break; }
  }
}


template <class G, class T>
void removeRandomEdgeByRank(G& a, vector<T>& r) {
  T v = T(rand()) / RAND_MAX, p = 0;
  for (auto u : a.vertices()) {
    if (v > p+r[u]) p += r[u];
    else { removeRandomEdge(a, u); break; }
  }
}
