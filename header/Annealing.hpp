#ifndef INCLUDE_ANNEALING_HPP
#define INCLUDE_ANNEALING_HPP

//アニーリング用の関数まとめ

#include <random>
#include <utility>
#include <cmath>
#include <map>
#include <string>
#include <algorithm>
#include "Graph.hpp"

using namespace std;

class Params {
    map<string, int> params;

    public:
    Params() {
        params["maxT"] = 1000;
        params["minT"] = 100;
        params["N"] = 100;
        params["seed"] = 1;
    } 

    Params(vector<string>& vec ) : Params() {
        if ( vec.size() % 2 == 1 ) {
            cout << "Not correct params size : " << vec.size() << endl;
            return;
        }

        for ( int i = 0; i < vec.size(); ++i ) {

            string key = vec[i];
            int value = stoi(vec[i+1]);
            if ( params.find(key) == params.end() ) {
                cout << "Not correct params :" << key << endl;
            }
            params[key] = value;
            i += 1;
        }
    }

    int get(const string& key) {
        return params[key]; 
    }

    void print() {
        cout << "[Paramaters]" << endl;
        for ( auto it = params.begin(); it != params.end(); ++it ) {
            cout << it->first << " : " << it->second << endl;
        }
    }
    
};

//node_no番のSwitchからエッジを1つ選択(Host辺を考慮) O(r)
Edge sel_edge( int node_no, Graph& graph, mt19937& mt ) {
    const Switch& sw = graph.getSwitch(G_no(0), Node_no(node_no));
    int h = ( sw.get_hsize() > 0 ) ? 1 : 0;
    int l = ( sw.get_loop() > 0 ) ? 1 : 0;
    int s = sw.get_r()-sw.get_hsize()-sw.get_loop();

    vector<Edge::edgeType> types(s, Edge::edgeType::SWITCH);
    if ( h ) types.push_back(Edge::edgeType::HOST);
    if ( l ) types.push_back(Edge::edgeType::LOOP);
    
    int no = uniform_int_distribution<int>(0, types.size()-1)(mt);
    Edge::edgeType type = types[no];
    int cnt = 0;
    if ( type == Edge::edgeType::HOST || type == Edge::edgeType::LOOP ) no = 0;
    Edge ans;

    for ( const Edge& edge : sw.getEdges() ) {
        if ( edge.getType() == type ) {
            if ( cnt == no ) {
                ans = edge;
                break;
            }
            ++cnt;
        }
    }
    return ans;
}

//グラフ上の2辺を選ぶ(Host辺やloop辺を考慮)
//種数は1前提
pair<Edge, Edge> select_edges2(Graph& graph, mt19937& mt ) {
    uniform_int_distribution<int> dist_node(0, graph.getSum_s()-1);

    int node1 = dist_node(mt);

    const Edge& e1 = sel_edge(node1, graph, mt);
    int forb_node1 = node1;
    int forb_node2 = -1;
    if ( e1.getType() == Edge::edgeType::SWITCH ) forb_node2 = e1.getNode().getNo();

    bool isHost = (e1.getType() == Edge::edgeType::HOST );
    while(1) {
        int node2 = dist_node(mt);
        if ( node2 == forb_node1 || node2 == forb_node2 ) continue;

        //選んだ辺の終端点が既存の頂点の場合弾く
        const Edge& e2 = sel_edge(node2, graph, mt);
        int to_node = e2.getNode().getNo();
        if ( to_node == forb_node1 || to_node == forb_node2 ) continue;

        if ( e2.getType() == Edge::edgeType::HOST && isHost ) continue;

        return make_pair(e1, e2);
    }
}

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