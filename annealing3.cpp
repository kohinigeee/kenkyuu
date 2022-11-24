//random選択　(ホスト辺考慮あり)
#include<iostream>
#include<vector>
#include<cmath>
#include<random>
#include<string>
#include<utility>

#include "./header/Debug.hpp"
#include "./header/functions.hpp"
#include "./header/Graph.hpp"
#include "./header/Annealing.hpp"
#include "./header/Annealing2.hpp"

using namespace std;
using ll = long long;

Graph kick(Graph& graph, mt19937& mt, int n ) {
    for ( int i = 0; i < n; ++i ) {
        pair<Edge,Edge> pee = select_edges3(graph,mt);
        graph.simple_swing(pee.first, pee.second);
        if ( !graph.isLinking() ) graph.back();
    }
    return graph;
}

int main(int argc, char** args)
{
    debug_off();

    vector<string> arg;
    vector<Graph> bests;
    vector<Params> vp;
    mt19937 grand_mt;

    for ( int i = 1; i < argc; ++i ) arg.push_back(args[i]);

    Params params = Params(arg);
    grand_mt.seed(params.get("seed"));

    try {
    Graph graph = Graph::make2(28, 32, 4, 1, 3);
    graph.toDot("graph1.dot", graph);

    Graph bestgraph = graph;
    for ( int i = 0; i < 10; ++i ) {
        params.set("seed", grand_mt());
        bestgraph = annealing(graph, params, select_edges3);
        bests.push_back(bestgraph);
        vp.push_back(params);

        graph = bestgraph;
        kick(graph, grand_mt, 100); 
    }

    for ( int i = 0; i < bests.size(); ++i ) {
        cout << endl;

        Graph::toDot("graph2.dot", bests[i]);
        cout << "No = " << i << endl; 
        graph_info_t info = bests[i].sumD();
        vp[i].print();
        cout << "Nubmer of switches : " << bests[i].getSum_s() << endl;
        cout << "[Results]" << endl;
        cout << "min_diam = " << info["diam"] << endl;
        cout << "min_hasple = " << bests[i].calcHaspl(info["sumd"]) << endl;
    }
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }

}