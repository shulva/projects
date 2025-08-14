#include <algorithm>
#include <iostream>
#include <queue>
#include <random>
#include <vector>

class vertex {
public:
  int vertex_num;
  int distance;
  vertex(int vertex_num, int distance);
};

class node {
public:
  std::vector<vertex> relationship;
  int s_distance;
  int id;
  node *prenode;
  bool color;

  friend bool operator<(node f1, node f2) {
    return f1.s_distance > f2.s_distance; // min
  }
};

class graph {
public:
  void graph_random_produce(int m0, int m, int t);
  void graph_weighted(int num);
  void graph_unweighted(int num); // via num to choose vertex
  void graph_show();

private:
  std::vector<std::vector<vertex>> graph_node_list;
};
