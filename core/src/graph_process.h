//
// Created by admin on 2019/3/11.
//

#ifndef CORE_GRAPH_PROCESS_H
#define CORE_GRAPH_PROCESS_H

#include <climits>
#include <vector>
#include <cstring>
#include <iostream>
#include <algorithm>

#define NO_EDGE         (INT_MAX)
#define NO_START_NODE   (0)
#define NO_END_NODE     (0)

#define NOT_VISITED (0)
#define VISITED     (1)
#define ALL_VISITED (2)

template<int N>
class Graph {
private:
    struct Edge {
        int u, v;
        int weight;
        Edge *next;
    };
    Edge **a;
    int *visited;
//    std::vector<int> route;
//    std::vector<int> best_route;

    // clear data in visited array
    void clear_visited() {
        std::memset(visited, 0, sizeof(int) * (N + 1));
    }

    // check is has circle in dfs
    bool has_circle_dfs(int start) {
        visited[start] = VISITED;
        for (Edge *edge = a[start]; edge != nullptr; edge = edge->next) {
            int next = edge->v;
            int status = visited[next];
            if (status == VISITED) {
                return true;
            } else if (status == ALL_VISITED) {
                continue;
            } else if (status == NOT_VISITED) {
                bool has = has_circle_dfs(edge->v);
                if (has) return true;
            }
        }
        visited[start] = ALL_VISITED;
        return false;
    }

    // dfs unique
    std::pair<int, std::vector<int> *> dfs_unique(int start_node, int end_node) {
        if (start_node == end_node) {
            std::vector<int> route{end_node};
            auto *route_ptr = new std::vector<int>(route);
            return std::make_pair(0, route_ptr);
        }

        visited[start_node] = VISITED;
        int result = NO_EDGE;
        std::vector<int> *result_route = nullptr;
        for (Edge *edge = a[start_node]; edge != nullptr; edge = edge->next) {
            int u = edge->u;
            int v = edge->v;
            int weight = edge->weight;
            if (visited[v] == VISITED) continue;
            std::pair<int, std::vector<int> *> next_return = dfs_unique(v, end_node);
            int next_result = next_return.first;
            std::vector<int> *next_route = next_return.second;
            if (next_result == NO_EDGE) continue;
            if ((result == NO_EDGE) || (result < next_result + weight)) {
                result = next_result + weight;
                result_route = new std::vector<int>(*next_route);
                result_route->emplace_back(u);
            }
        }
        visited[start_node] = NOT_VISITED;
        return std::make_pair(result, result_route);
    }

public:
    // constructor
    Graph() {
        a = new Edge *[N + 1];
        for (int i = 0; i <= N; i++) {
            a[i] = nullptr;
        }
        visited = new int[N + 1];
        clear_visited();
    }

    // destructor
    ~Graph() {
        for (int i = 0; i <= N; i++) {
            Edge *edge = a[i];
            while (edge != nullptr) {
                Edge *next = edge->next;
                delete edge;
                edge = next;
            }
            a[i] = nullptr;
        }
        delete[] a;
        delete[] visited;
    }

    // add a new edge into graph
    void add_edge(int u, int v, int weight) {
        Edge *edge = new Edge;
        edge->u = u;
        edge->v = v;
        edge->weight = weight;
        edge->next = a[u];
        a[u] = edge;
    }

    // check if there is circle in graph
    bool has_circle() {
        clear_visited();
        for (int i = 1; i <= N; i++) {
            if (visited[i] == ALL_VISITED) continue;
            bool has = has_circle_dfs(i);
            if (has) return true;
        }
        return false;
    }

    std::vector<int> *get_longest_path() {
        return get_longest_path(NO_START_NODE, NO_END_NODE);
    }

    std::vector<int> *get_longest_path_with_start(int start_node) {
        return get_longest_path(start_node, NO_END_NODE);
    }

    std::vector<int> *get_longest_path_with_end(int end_node) {
        return get_longest_path(NO_START_NODE, end_node);
    }

    std::vector<int> *get_longest_path(int start_node, int end_node) {
        if (has_circle()) {
            return nullptr;
        }

        int **route;
        route = new int *[N + 1];
        for (int i = 0; i <= N; i++) {
            route[i] = new int[N + 1];
        }

        int **dist;
        dist = new int *[N + 1];
        for (int i = 0; i <= N; i++) {
            dist[i] = new int[N + 1];
        }

        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= N; j++) {
                route[i][j] = 0;
                if (i == j) {
                    dist[i][j] = 0;
                } else {
                    dist[i][j] = NO_EDGE;
                }
            }
        }
        for (int i = 1; i <= N; i++) {
            for (Edge *edge = a[i]; edge != nullptr; edge = edge->next) {
                int u = edge->u;
                int v = edge->v;
                if ((dist[u][v] == NO_EDGE) || (dist[u][v] < edge->weight)) {
                    dist[u][v] = edge->weight;
                    route[u][v] = v;
                }
            }
        }
        for (int k = 1; k <= N; k++) {
            for (int i = 1; i <= N; i++) {
                if (dist[i][k] == NO_EDGE) continue;
                for (int j = 1; j <= N; j++) {
                    if (dist[k][j] == NO_EDGE) continue;
                    if ((dist[i][j] == NO_EDGE) || (dist[i][j] < dist[i][k] + dist[k][j])) {
                        dist[i][j] = dist[i][k] + dist[k][j];
                        route[i][j] = route[i][k];
                    }
                }
            }
        }

        int max_u = 0;
        int max_v = 0;
        int max_dist = NO_EDGE;
        for (int i = 1; i <= N; i++) {
            if ((start_node != NO_START_NODE) && (start_node != i)) continue;
            for (int j = 1; j <= N; j++) {
                if ((end_node != NO_END_NODE) && (end_node != j)) continue;
                if (dist[i][j] == NO_EDGE) continue;
                if ((max_dist == NO_EDGE) || (dist[i][j] > max_dist)) {
                    max_dist = dist[i][j];
                    max_u = i;
                    max_v = j;
                }
            }
        }

        if (max_dist != NO_EDGE) {
            int u = max_u;
            int v = max_v;
            std::vector<int> list{u};
            while (u != v) {
                int next = route[u][v];
                list.insert(list.end(), next);
                u = next;
            }
            return new std::vector<int>(list);
        } else {
            return nullptr;
        }
    }

    std::vector<int> *search_longest_unique_path(int start_node, int end_node) {
        std::pair<int, std::vector<int> *> result = dfs_unique(start_node, end_node);
        int ret = result.first;
        if (ret == NO_EDGE) {
            return nullptr;
        } else {
            std::vector<int> *raw_route = result.second;
            std::vector<int> route = *raw_route;
            std::reverse(route.begin(), route.end());
            return new std::vector<int>(route);
        }
    }
};


#endif //CORE_GRAPH_PROCESS_H