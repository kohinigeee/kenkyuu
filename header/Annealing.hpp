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
        params["s"] = 0;
        params["h"] = 0;
        params["r"] = 0;
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

    void set(const string& key, long long value) {params[key] = value;}

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

//Graph gからEdgeをぬきだしたリストを生成(すべての辺を抜き出す)
vector<Edge> makeEdgeList_normal(Graph& g){
    vector<Edge> edges;

    for ( int i = 0; i < g.getSum_g(); ++i ) {
        const GraphPart& gp = g.getPart(G_no(i));
        int ssize = gp.get_ssize();
        for ( int j = 0; j < ssize; ++j ) {
            const Switch& sw = gp.get_switch(Node_no(j));
           for ( Edge edge : sw.getEdges() ) {
            if ( edge.getStatus() == Edge::edgeStatus::LOCKED ) continue;
            Edge::edgeType type = edge.getType();
            if ( type == Edge::edgeType::HOST ) {
                edges.push_back(edge);
                continue;
            }
            if ( type == Edge::edgeType::LOOP ) {
                edges.push_back(edge);                 
                continue;
            }
            if ( edge.getNode().getNo() <= j ) continue;
            edges.push_back(edge);
           } 
        }
    }
    return edges;
}

//Graph gからEdgeをぬきだしたリストを生成( 同スイッチからのホスト・ループ辺は1つまで)
vector<Edge> makeEdgeList_hl(Graph& g){
    vector<Edge> edges;
    const int hosts_limit = 1;
    const int loops_limit = 1;

    for ( int i = 0; i < g.getSum_g(); ++i ) {
        const GraphPart& gp = g.getPart(G_no(i));
        int ssize = gp.get_ssize();
        for ( int j = 0; j < ssize; ++j ) {
            const Switch& sw = gp.get_switch(Node_no(j));
            int h = 0; int loop = 0;
           for ( Edge edge : sw.getEdges() ) {
            if ( edge.getStatus() == Edge::edgeStatus::LOCKED ) continue;
            Edge::edgeType type = edge.getType();
            if ( type == Edge::edgeType::HOST && h < hosts_limit ) {
                edges.push_back(edge); ++h;
                continue;
            }
            if ( type == Edge::edgeType::LOOP && loop < loops_limit ) {
                edges.push_back(edge); ++loop;
                continue;
            }
            if ( edge.getNode().getNo() <= j ) continue;
            edges.push_back(edge);
           } 
        }
    }
    return edges;
}

//種数は1前提
//各エッジについて均等 O(nr)
pair<Edge, Edge> select_edges3(Graph& graph, mt19937& mt, vector<Edge>(*makeEdge)(Graph& g)) {
    vector<Edge> edges = makeEdge(graph);
    uniform_int_distribution<int> dist_edge(0, edges.size()-1);

    const Edge& e1 = edges[dist_edge(mt)];
    const Edge& e_to = graph.getEdge(e1);
    Node_no node_no1 = e_to.getNode();

    int forb_node1 = node_no1.getNo();
    int forb_node2 = -1;
    if ( e1.getType() == Edge::edgeType::SWITCH ) forb_node2 = e1.getNode().getNo();

    bool isHost = (e1.getType() == Edge::edgeType::HOST );

    while(1) {
        const Edge& e2 = edges[dist_edge(mt)];
        const Edge& e2_to = graph.getEdge(e2);

        int node2 = e2_to.getNode().getNo();
        if ( node2 == forb_node1 || node2 == forb_node2 ) continue;

        int to_node2 = e2.getNode().getNo();
        if ( to_node2 == forb_node1 || to_node2 == forb_node2 ) continue;
        if ( e2.getType() == Edge::edgeType::HOST && isHost ) continue;

        return make_pair(e1, e2);
    }
}

pair<Edge, Edge> select_edges_noraml(Graph& graph, mt19937& mt) {
    return select_edges3(graph, mt, makeEdgeList_normal);
}

pair<Edge, Edge> select_edges_hl(Graph& graph, mt19937& mt) {
    return select_edges3(graph, mt, makeEdgeList_hl);
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

// 直径も評価に含む評価関数
//@T: 現在の焼きなましの温度
double eval1(Graph& newGraph, graph_info_t& newInfo,  graph_info_t& prevInfo, double T ) {
    double prev_haspl = newGraph.calcHaspl(prevInfo["sumd"]);
    double new_haspl = newGraph.calcHaspl(newInfo["sumd"]);
    long long prev_diam = prevInfo["diam"];
    long long new_diam = newInfo["diam"];

    if ( new_diam < prev_diam ) return 1.5;

    double delta = (new_haspl-prev_haspl)*newGraph.getSum_s()*(newGraph.getSum_h()-1);
    if ( delta < 0 ) return 1.5;

    double prob = pow(M_E, -(delta/T));

    return prob;
}

pair<Edge,Edge> bias(Graph& graph, pair<Edge,Edge>& p, mt19937& mt ) {
    uniform_real_distribution<double> dis(0, 1);
    pair<Edge, Edge> ans;
    Edge e1_to = p.first;
    Edge e2_to = p.second;

    Edge e1_from = graph.getEdge(e1_to);
    Edge e2_from = graph.getEdge(e2_to);

    int na, nb, nc, nd;

    if ( e1_from.getType() == Edge::edgeType::HOST ) na = 1;
    else na = graph.getSwitch(e1_from.getG(), e1_from.getNode()).get_ssize();
    if ( e1_to.getType() == Edge::edgeType::HOST ) nb = 1;
    else nb = graph.getSwitch(e1_to.getG(), e1_to.getNode()).get_ssize();

    double prob;
    if ( na+nb == 0 ) prob = 0;
    else prob = double(nb)/(na+nb);

    if ( dis(mt) <= prob ) ans.first = e1_from;
    else ans.first = e1_to;

    if ( e2_from.getType() == Edge::edgeType::HOST ) nc = 1;
    else nc = graph.getSwitch(e2_from.getG(), e2_from.getNode()).get_ssize();
    if ( e2_to.getType() == Edge::edgeType::HOST ) nd = 1;
    else nd = graph.getSwitch(e2_to.getG(), e2_to.getNode()).get_ssize();

    if ( nd+nc == 0 ) prob = 0;
    else prob = double(nd)/(nc+nd);

    if ( dis(mt) <= prob ) ans.second = e2_from;
    else ans.second = e2_to;

   return ans; 
}

#endif 