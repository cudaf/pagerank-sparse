#pragma once
#include <vector>
#include <unordered_map>

using std::vector;
using std::unordered_map;




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
  }
  return a;
}

template <class G>
auto destinationIndices(G& x) {
  return destinationIndices(x, x.vertices());
}