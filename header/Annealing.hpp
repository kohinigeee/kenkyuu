#ifndef INCLUDE_ANNEALING_HPP
#define INCLUDE_ANNEALING_HPP

//アニーリング用の関数まとめ

#include <random>
#include <utility>
#include "Graph.hpp"

using namespace std;

//グラフ上の2辺を選ぶ (端点が片方でも同じ辺のペアは弾く)
//種数は1前提
pair<Edge, Edge> select_edges(Graph& graph, mt19937& mt ) {
    uniform_int_distribution<int> dist_node(0, graph.getSum_s()-1);
    uniform_int_distribution<int> dist_edge(0, graph.get_r()-1);

    int node1 = dist_node(mt);
    int edge1 = dist_edge(mt);

    // cout << "node1 = " << node1 << ", edge1 =" << edge1 << endl;

    const Edge& e1 = graph.getEdge(Edge::edgeType::SWITCH, G_no(0), Node_no(node1), Edge_no(edge1));
    int forb_node1 = node1;
    int forb_node2 = -1;
    if ( e1.getType() == Edge::edgeType::SWITCH ) forb_node2 = e1.getNode().getNo();

    bool isHost = (e1.getType() == Edge::edgeType::HOST );

    while(1) {
        int node2 = dist_node(mt);
        if ( node2 == forb_node1 || node2 == forb_node2 ) continue;
        int edge2 = dist_edge(mt);

        // cout << "node2 = " << node2 << ", edge2 = " << edge2 << endl;

        //選んだ辺の終端点が既存の頂点の場合弾く
        const Edge& e2 = graph.getEdge(Edge::edgeType::SWITCH, G_no(0), Node_no(node2), Edge_no(edge2));
        int to_node = e2.getNode().getNo();
        if ( to_node == forb_node1 || to_node == forb_node2 ) continue;

        if ( e2.getType() == Edge::edgeType::HOST && isHost ) continue;

        return make_pair(e1, e2);
    }
}
#endif 