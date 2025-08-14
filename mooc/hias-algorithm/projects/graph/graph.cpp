#include "graph.h"
#define inf 214748367

vertex::vertex(int vertex_num, int distance) {
  this->vertex_num = vertex_num;
  this->distance = distance;
}

void graph::graph_random_produce(int m0, int m, int t) {

  std::vector<vertex> *node = new std::vector<vertex>[m0 + t];
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, 2 * m0 + 2 * t * m);

  for (int i = 0; i < m0; i++) {
    node[i].push_back(vertex((i + 1) % m0, 1));
    node[i].push_back(vertex((m0 - 1 + i) % m0, 1));
    graph_node_list.push_back(node[i]);
  }

  int num = 0;
  int *count = new int[m0 + t];

  for (int i = m0; i < t + m0; i++) { // t times
                                      //
    count[0] = graph_node_list[0].size();
    for (int j = 1; j < i; j++) {
      count[j] = 0;
      count[j] =
          count[j - 1] + graph_node_list[j].size(); // calculate before random
    }

    for (int k = 0; k < m; k++) {
      num = distrib(gen) % (2 * (m0 + m * (i - m0))); // random edge
                                                      //

      for (int j = 0; j < i; j++) {
        if (num <= count[j] - 1) {

          if (std::find_if(node[i].begin(), node[i].end(),
                           [&](const auto &val) {
                             return val.vertex_num == j;
                           }) != node[i].end()) {
            k--;
            break;
          }

          graph_node_list[j].push_back(vertex(i, 1));
          node[i].push_back(vertex(j, 1));
          break;
        }
      }
    }
    graph_node_list.push_back(node[i]);
  }
}
void graph::graph_show() {

  int loop = graph_node_list.size();
  int loop_2 = 0;

  for (int i = 0; i < loop; i++) {
    loop_2 = graph_node_list[i].size();
    std::cout << i;
    for (int j = 0; j < loop_2; j++) {
      std::cout << "-->" << graph_node_list[i][j].vertex_num;
    }
    std::cout << std::endl;
  }
}

void graph::graph_weighted(int num) {

  std::priority_queue<node> Q;
  node *node_list = new node[graph_node_list.size()];
  node *node_list2 = new node[graph_node_list.size()];
  std::vector<int> *tree_print = new std::vector<int>[graph_node_list.size()];

  for (int i = 0; i < graph_node_list.size(); i++) { // initialize single source
    node_list[i].s_distance = inf;
    node_list[i].relationship = graph_node_list[i];
    node_list[i].prenode = NULL;
    node_list[i].id = i;
    node_list[i].color = false;
  }

  node_list[num].s_distance = 0;
  Q.push(node_list[num]);

  int count = 0;

  while (!Q.empty()) {
    node_list2[count] = Q.top();
    Q.pop();

    for (int i = 0; i < node_list2[count].relationship.size(); i++) {
      if (node_list[node_list2[count].relationship[i].vertex_num].color ==
          false) {
        node_list[node_list2[count].relationship[i].vertex_num].color = true;
        if (node_list[node_list2[count].relationship[i].vertex_num].s_distance >
            node_list2[count].s_distance +
                node_list2[count]
                    .relationship[i]
                    .distance) { // w.dist > v.dist + cvw
                                 //

          node_list[node_list2[count].relationship[i].vertex_num].prenode =
              &node_list2[count];

          node_list[node_list2[count].relationship[i].vertex_num].s_distance =
              node_list2[count].s_distance +
              node_list2[count]
                  .relationship[i]
                  .distance; // w.dist = v.dist + cvw
          Q.push(node_list[node_list2[count].relationship[i].vertex_num]);
        }
      }
    }
    count++;
  }
  std::cout << "-----------------------------------------------------"
            << std::endl;

  for (int i = 0; i < graph_node_list.size(); i++) {
    if (node_list[i].prenode != NULL) {
      tree_print[i].push_back(node_list[i].prenode->id);
      tree_print[node_list[i].prenode->id].push_back(i);
    }
  }

  int loop = graph_node_list.size();
  int loop_2 = 0;
  int count2 = 0;

  for (int i = 0; i < loop; i++) {
    loop_2 = tree_print[i].size();
    std::cout << i;
    for (int j = 0; j < loop_2; j++) {
      std::cout << "-->" << tree_print[i][j];
    }
    std::cout << "  dist:" << node_list[count2].s_distance << std::endl;
    count2++;
  }
}
void graph::graph_unweighted(int num) {

  std::queue<node> Q;
  node *node_list = new node[graph_node_list.size()];
  node *node_list2 = new node[graph_node_list.size()];
  std::vector<int> *tree_print = new std::vector<int>[graph_node_list.size()];

  for (int i = 0; i < graph_node_list.size(); i++) {
    node_list[i].s_distance = -1;
    node_list[i].relationship = graph_node_list[i];
    node_list[i].prenode = NULL;
    node_list[i].id = i;
  }

  node_list[num].s_distance = 0;
  Q.push(node_list[num]);
  int count = 0;

  while (!Q.empty()) {
    node_list2[count] = Q.front();
    Q.pop();

    for (int i = 0; i < node_list2[count].relationship.size(); i++) {
      if (node_list[node_list2[count].relationship[i].vertex_num].s_distance ==
          -1) {
        node_list[node_list2[count].relationship[i].vertex_num].s_distance =
            node_list2[count].s_distance +
            node_list2[count].relationship[i].distance;

        node_list[node_list2[count].relationship[i].vertex_num].prenode =
            &node_list2[count];
        Q.push(node_list[node_list2[count].relationship[i].vertex_num]);
      }
    }
    count++;
  }

  for (int i = 0; i < graph_node_list.size(); i++) {
    if (node_list[i].prenode != NULL) {
      tree_print[i].push_back(node_list[i].prenode->id);
      tree_print[node_list[i].prenode->id].push_back(i);
    }
  }

  int loop = graph_node_list.size();
  int loop_2 = 0;
  int count2 = 0;

  std::cout << "-----------------------------------------------------"
            << std::endl;
  for (int i = 0; i < loop; i++) {
    loop_2 = tree_print[i].size();
    std::cout << i;
    for (int j = 0; j < loop_2; j++) {
      std::cout << "-->" << tree_print[i][j];
    }
    std::cout << "  dist:" << node_list[count2].s_distance << std::endl;
    count2++;
  }
}
