#ifndef INCLUDE_DIVIDEHILLCLIMB_HPP
#define INCLUDE_DIVIDEHILLCLIMB_HPP

#include "Graph.hpp"
#include "Divide.hpp"
#include "DivideAnnealing.hpp"
#include "HillClimb.hpp"

using namespace std;

using divideHillclimb_func = Graph(*)(Graph&, const double alpha, const int seed, select_func, long long st_time, History&);

//分割山登り法
//@dinfo: 分割時の各グループの情報
//@n    : 遷移回数
//@seed : seed値
//@select: エッジの選択関数
//@hillclimb: 山登りの方法(キックの仕方)

Graph divideHillClimb( const divideInfo& dinfo, const double alpha, const int seed, select_func select, divideHillclimb_func hillclimb, int frees = 0 ) {
    vector<GraphPart> gps;
    mt19937 mt;
    const int r = dinfo.r;
    mt.seed(seed);

    for ( int i = 0; i < dinfo.get_size(); ++i ) {
        // cout << "[divideHillClimb] :: i = " << i << endl;
        Graph::set_seed(mt());
        const int s_under = dinfo.nodes[i];
        const int h_under = dinfo.hosts[i];
        const int r_under = dinfo.ports[i];

        Graph graph = Graph::make3(s_under, h_under, dinfo.r, 1, r_under);
        //LOG
        // graph.print("Under_" + to_string(i));


        Graph best = hillclimb(graph, alpha, mt(), select, 0, defo_history);
        gps.push_back(best.getPart(G_no(0)));

        //Log
    //    gps[i].print("Under_" + to_string(i));
    }
    
    Graph upper = makeUpperGraph(dinfo.ports, r, frees);
    Graph best = hillclimb(upper, alpha, mt(), select, 0, defo_history);

    // best.print();
    // cout << "finished makeUpperGraph" << endl;

    Graph ans = reconstruct(gps, best, dinfo.ports, r, frees);
    // cout << "finished reconstruct" << endl;
    ans = ans.integrate();
    // cout << "finished integrate" << endl;
    if ( !(ans.isLinking()) ) {
        string msg = "[Error] divieAnnealing:: made not Linking Graph";
        cout << msg << endl;
        exit(1);
    }
    return ans;
}


//振り分け方：整数計画利用
Graph divideHillclimb_1(int s, int h, int r, const int seed) {
    const double beta = 0.45;
    const double alpha = 0.6;

    divideInfo dinfo = divideInfo::makeDivide_ports(s, h, r);
    Graph best = divideHillClimb(dinfo, alpha, seed, select_edges_hl, hillclimb);
    return best;
}

Graph divideHillclimb_2( int s, int h, int r, const int seed) {
    divideInfo dinfo = divideInfo::makeDivide_normal(s, h, r);
    const double alpha = 0.6;

    Graph best = divideHillClimb(dinfo, alpha, seed, select_edges_hl, hillclimb);
    return best;
}
#endif