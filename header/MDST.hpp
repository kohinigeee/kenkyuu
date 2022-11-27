#ifndef INCLUDE_MDST_HPP
#define INCLUDE_MDST_HPP

#include<vector>
#include<utility>
#include"./Graph.hpp"

using namespace std;

using mdst_tree_t = vector<pair<Edge, Edge>>;

//頂点a,b間のセンターを求める
//戻り値： first:整数部分, second:センターの位置 ( 0:a側, 1:b側, 2:中心)
//引数  @d: 距離行列
pair<long long, int> calcEdgeCenter(int a, int b, vector<vector<long long>>& d) {
    long long ming = 0;
    long long set = 0;

    const int abig = 1;
    const int bbig = 1<<1;

    for ( int i = 0; i < d.size(); ++i ) {
        long long tmpv = min(d[a][i], d[b][i]);
        if ( tmpv > ming ) {
            ming = tmpv; set = 0;
        }
        if ( tmpv == ming ) {
            if ( d[a][i] < d[b][i] ) set = set | bbig;
            if ( d[a][i] > d[b][i] ) set = set | abig;
        }
    }

    if ( set == 0 ) return make_pair(ming, 0);
    if ( set == abig ) return make_pair(ming, 1);
    if ( set == bbig ) return make_pair(ming, 0);
    if ( set == abig+bbig) return make_pair(ming, 2);
    return make_pair(ming, 10000);
}

//最小直径全域木を求める
//graphの種数は1前提
vector<pair<Edge,Edge>> mdst(Graph& graph) {
    vector<vector<long long>> d;
    pair<long long, int> center;
    pair<int, int> nodes; //first:頂点a, second:頂点b

    if ( !graph.isLinking() ) {
        string msg = "[Error] mdst:: graph is not linking";
        cout << msg << endl;
        throw new IregalValueException(msg);
    }

    for ( int i = 0; i < graph.getSum_s(); ++i ) {
        vector<long long> tmpd = graph.bfs(i);
        d.push_back(tmpd);
    }

    center = make_pair((1LL<<60), 10);

    for ( int i = 0; i < graph.getSum_s(); ++i ) {
        const Switch& sw = graph.getSwitch(G_no(0), Node_no(i));
        for ( const Edge& edge: sw.getEdges() ) {
            if ( edge.getType() != Edge::edgeType::SWITCH ) continue;
            int to = edge.getNode().getNo();
            if ( to <= i ) continue;

            pair<long long, int> tmpCenter = calcEdgeCenter(i, to, d);
            if ( tmpCenter < center ) {
                center = tmpCenter;
                nodes = make_pair(i, to);
            }
        }
    }

    vector<pair<Edge,Edge>> tree;

    vector<long long> tmpd(graph.getSum_s(), -1);
    queue<int> que;

    if ( center.second == 0 ) {
        tmpd[nodes.first] = 0;
        que.push(nodes.first);
    }
    if ( center.second == 1 ) {
        tmpd[nodes.second] = 0;
        que.push(nodes.second);
    }
    if ( center.second == 2 ) {
        tmpd[nodes.first] = 0;
        tmpd[nodes.second] = 0;
        que.push(nodes.first);
        que.push(nodes.second);
    }

    G_no g_no = G_no(0);
    while(!que.empty()) {
        int top = que.front(); que.pop();

        const Switch& sw = graph.getSwitch(g_no, Node_no(top));
        for ( Edge edge : sw.getEdges() ) {
            if ( edge.getType() != Edge::edgeType::SWITCH ) continue;
            int to = edge.getNode().getNo();

            if ( tmpd[to] != -1 ) continue;
            
            tmpd[to] = tmpd[top]+1;
            que.push(to);
            Edge to_edge = graph.getEdge(edge);
            tree.push_back(make_pair(edge, to_edge));
        }
    }
    return tree;
}

//MDSTGraphを生成
//種数1前提
Graph makeMDTgraph(Graph& graph ) {
    mdst_tree_t tree = mdst(graph);

    Graph graphT = graph;
    graphT.loopInit();

    for ( auto p : tree ) {
        Edge e1 = p.first; Edge e2 = p.second;

        Switch& s1 = graphT.getSwitch(e2.getG(), e2.getNode());
        s1.setEdge(e2.getEdge(), e1);

        Switch& s2 = graphT.getSwitch(e1.getG(), e1.getNode());
        s2.setEdge(e1.getEdge(), e2);
    }
    return graphT;
}



#endif