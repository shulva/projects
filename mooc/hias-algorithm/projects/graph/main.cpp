#include "graph.h"
#include <ctime>

int main() {
  graph G;
  int t;
  std::cin >> t;
  G.graph_random_produce(3, 2, t);
  G.graph_show();
  G.graph_unweighted(9);
  clock_t start = std::clock();
  G.graph_weighted(9);
  clock_t end = std::clock();
  std::cout << end - start;

  return 0;
}
