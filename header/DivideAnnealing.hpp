#ifndef INCLUDE_DEVIDEANNEALING_HPP
#define INCLDUE_DEVIDEANNEALING_HPP
#include<iostream>
#include<iostream>
#include<vector>
#include<cmath>
#include<random>
#include<string>
#include<utility>


#include "Debug.hpp"
#include "functions.hpp"
#include "Nodes.hpp"
#include "Edge.hpp"
#include "Graph.hpp"
#include "GraphPart.hpp"
#include "Annealing.hpp"
#include "Annealing2.hpp"

using namespace std;
using ll = long long;

//values_r　：各下位グラフの上位グラフ用にとっているポートの数
//r : スイッチのポート数
//frees : 各下位グラフと接続しないスイッチの数
Graph makeUpperGraph(vector<int>& values_r, int r, int frees = 0) {
    int maxr = 0;
    for ( auto v : values_r ) maxr = max(v, maxr);
    if ( maxr <= 0 ) { //上位用ポートが0個の下位グラフが存在する場合はエラー
        string msg = "[Error] makeUpperGraph:: there is have node which have no edges";
        cout << msg << endl;
        throw IregalValueException(msg);
    }

    if ( frees > 0 ) maxr = max(r, maxr);
    const int s = values_r.size()+frees;
    const int h = values_r.size();

    GraphPart gp(s, 0, maxr+1, G_no(0));

    vector<int> cnts(s, 0);
    vector<int> rmaxs(s, 0); //各スイッチの使えるポートの限界数

    for ( int i = 0; i < values_r.size(); ++i ) rmaxs[i] = values_r[i]+1; //＋1はホスト用
    for ( int i = 0; i < frees; ++i ) rmaxs[values_r.size()+i] = r;

    //禁止辺のロック
    for ( int i = 0; i < gp.get_ssize(); ++i ) {
        int forbit_maxr= gp.get_r()-rmaxs[i];
        Switch& sw = gp.get_switch(Node_no(i));

        for ( int j = 0; j < forbit_maxr; ++j ) {
            int edge_no = gp.get_r()-1-j;
            sw.setEdge(Edge_no(edge_no), Edge::makeToLoop(G_no(0), Node_no(i), Edge_no(edge_no), Edge::edgeStatus::LOCKED));
        }
    }

    //ホスト辺の割り振り
    for ( int i = 0; i < h; ++i ) {
        gp.addHost(Node_no(i), Edge_no(cnts[i]));
        gp.get_switch(Node_no(i)).getEdge(Edge_no(cnts[i])).setStatus(Edge::edgeStatus::LOCKED);
        gp.get_host(Node_no(i)).getEdge().setStatus(Edge::edgeStatus::LOCKED);
        ++cnts[i];
    }

    G_no g = G_no(0);
    for ( int i = 0; i < gp.get_ssize(); ++i ) {
        if ( cnts[i] >= rmaxs[i] ) continue;

        int node1 = i; 
        int rnode= (i+1)%gp.get_ssize();
        Node_no node_no = Node_no(node1);
        Node_no r_no = Node_no(rnode);
        
        if ( cnts[rnode] < rmaxs[rnode] && cnts[node1] < rmaxs[node1] ) {
            gp.get_switch(node_no).setEdge(cnts[node1], Edge::makeToSwitich(g, r_no, Edge_no(cnts[rnode])));
            gp.get_switch(r_no).setEdge(cnts[rnode], Edge::makeToSwitich(g, node_no, Edge_no(cnts[node1])));
            ++cnts[rnode]; ++cnts[node1];
           }

        for ( int i = 0; i < gp.get_ssize(); ++i ) {
            Switch& sw = gp.get_switch(Node_no(i));
            for ( int j = cnts[i]; j < rmaxs[i]; ++j ) {
                sw.setEdge(Edge_no(j), Edge::makeToLoop(g, Node_no(i), Edge_no(j)));
            }
        }
    }
    vector<GraphPart> vec;
    vec.push_back(gp);
    Graph graph = Graph::makeFromParts(gp.get_r(), vec);
    
    graph.linkLoops();
    if ( !graph.isLinking() || !graph.isPropGraph() )  {
        string msg = "[Error] makeUpperGraph:: can't linking graph";
        cout << msg << endl;
        throw IregalValueException(msg);
    }
    return graph;
}

//UnderGraphとUpperGraphからグラフを再構築
Graph reconstruct(const vector<GraphPart>& parts, Graph& upper, vector<int>& ports, int r, int frees = 0 ) {
    vector<GraphPart> tmp = parts;
    vector<int> ports2 = ports;

    //free Switch用のGraphPart
    for ( int i = 0; i < frees; ++i ) {
        GraphPart gp(1, 0, r, G_no(parts.size()+i));
        for ( int j = 0; j < gp.get_ssize(); ++j ) {
            Switch& sw = gp.get_switch(Node_no(j));
            for ( int k = 0; k < r; ++k ) {
                sw.setEdge(Edge_no(k), Edge::makeToLoop(G_no(gp.get_gno()), Node_no(j), Edge_no(k), Edge::edgeStatus::LOCKED));
            }
        }
        tmp.push_back(gp);
        ports2.push_back(r);
    }

    Graph graph = Graph::makeFromParts(r, tmp);
    vector<vector<Edge>> edges(tmp.size());

    // cout << "reconstct:: collecting edges" << endl;
    vector<int> cnt(tmp.size(), 0);
    for ( int i = 0; i < ports2.size(); ++i ) {
        for ( const Switch& sw : graph.getPart(G_no(i)).get_switchs() ) {
            for ( const Edge e : sw.getEdges() ) {
                if ( e.getStatus() == Edge::edgeStatus::LOCKED && e.getType() == Edge::edgeType::LOOP ) {
                    edges[i].push_back(e);
                }
            }
        }
    }

    // cout << "reconstruct:: constructing graph" << endl;
    //freeスイッチが0個の場合
    GraphPart& gp = upper.getPart(G_no(0));
    for ( int i = 0; i < gp.get_ssize(); ++i  ) {
        const Switch& sw = gp.get_switch(Node_no(i));
        for ( const Edge& e : sw.getEdges() ) {
            if ( e.getStatus() == Edge::edgeStatus::LOCKED ) continue;
            int node2 = e.getNode().getNo();
            if ( i >= node2 ) continue;
            
            // cout << "node = " << i << endl;
            // e.print("Recontstuct");
            Edge e1 = edges[i][cnt[i]];
            Edge e2 = edges[node2][cnt[node2]];

            e1.setType(Edge::edgeType::SWITCH);
            e2.setType(Edge::edgeType::SWITCH);
            graph.getSwitch(e1.getG(), e1.getNode()).setEdge(e1.getEdge(), e2);
            graph.getSwitch(e2.getG(), e2.getNode()).setEdge(e2.getEdge(), e1);
            cnt[i]++; cnt[node2]++;
        }
    }
    // cout << "finishd" << endl;
    return graph;
}

//分割アニーリングを実行
//@params : paramater
//@groups : 分割するグループの数
//@values : 各グループのスイッチ数とホストの振り分け ( first:スイッチ数, second:ホスト数)
//@ports : 各グループが上位グラフ用に所持しているポートの数
//@frees : 上位グラフに利用されるスイッチの数
Graph divideAnnealing(Params params, int groups, int r, vector<pair<int,int>>& values, vector<int>& ports, int frees = 0) {
    vector<GraphPart> gps;
    mt19937 mt;
    mt.seed(params.get("seed"));

    for ( int i = 0; i < groups; ++i ) {
        Graph::set_seed(mt());
        Graph graph = Graph::make3(values[i].first, values[i].second, r, 1, ports[i]);
        Params tmp_params = params;
        params.set("seed", mt());

        Graph best = annealing(graph, params, select_edges_noraml);
        gps.push_back(best.getPart(G_no(0)));
    }
    Graph upper = makeUpperGraph(ports, r, frees);

    Params upper_parmas = params;
    params.set("seed", mt());
    // cout << "start upper annealing" << endl;
    Graph best = annealing(upper, params, select_edges_noraml);

    Graph ans = reconstruct(gps, best, ports, r, frees);
    ans = ans.integrate();
    if ( !(ans.isLinking()) ) {
        string msg = "[Erorr] divideAnnealing:: made not Linking Graph";
        cout << msg << endl;
        throw IregalValueException(msg);
    }

    return ans;
}

//分割アニーリング
// @free: 上位用グラフのスイッチの個数
Graph divideAnnealing( Params& params, const int s, const int h, const int r, int free=0 ) {
    
    int groups = int(sqrt(s)); //グループの数
    if ( groups*groups < s ) ++groups;
    vector<pair<int,int>> values;
    vector<int> ports;

    values = vector<pair<int,int>>(groups, make_pair(s/groups, h/groups));
    ports = vector<int>(groups, r-1);
    vector<GraphPart> gps;

    for ( int i = 0; i < s%groups; ++i ) values[i].first += 1;
    for ( int i = 0; i < h%groups; ++i ) values[i].second += 1;

    Graph ans = divideAnnealing(params, groups, r, values, ports, free);
    ans.linkLoops(true);

    return ans;
}
#endif