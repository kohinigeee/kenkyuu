#ifndef INCLUDE_DIVIDEHILLCLIMB_HPP
#define INCLUDE_DIVIDEHILLCLIMB_HPP

#include "Graph.hpp"
#include "Divide.hpp"
#include "DivideAnnealing.hpp"
#include "HillClimb.hpp"

using namespace std;

//分割山登り法
//@dinfo: 分割時の各グループの情報
//@n    : 遷移回数
//@seed : seed値
//@select: エッジの選択関数
//@hillclimb: 山登りの方法(キックの仕方)

Graph divideHillClimb( const divideInfo& dinfo, const int n, const int seed, select_func select, hillclimb_func hillclimb, int frees = 0 ) {
    vector<GraphPart> gps;
    mt19937 mt;
    const int r = dinfo.r;
    mt.seed(seed);

    for ( int i = 0; i < dinfo.get_size(); ++i ) {
        cout << "[divideHillClimb] :: i = " << i << endl;
        Graph::set_seed(mt());
        const int s_under = dinfo.nodes[i];
        const int h_under = dinfo.hosts[i];
        const int r_under = dinfo.ports[i];

        Graph graph = Graph::make3(s_under, h_under, dinfo.r, 1, r_under);
        Graph best = hillclimb(graph, n, mt(), select);
        gps.push_back(best.getPart(G_no(0)));
    }

    for ( int i = 0; i < dinfo.get_size(); ++i ) {
        gps[i].print("Gps_" + to_string(i));
    }
    
    Graph upper = makeUpperGraph(dinfo.ports, r, frees);
    Graph best = hillclimb(upper, n, mt(), select);

    best.print();
    cout << "finished makeUpperGraph" << endl;

    Graph ans = reconstruct(gps, best, dinfo.ports, r, frees);
    cout << "finished reconstruct" << endl;
    ans = ans.integrate();
    cout << "finished integrate" << endl;
    if ( !(ans.isLinking()) ) {
        string msg = "[Error] divieAnnealing:: made not Linking Graph";
        cout << msg << endl;
        exit(1);
    }
    return ans;
}
#endif