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

using namespace std;
using ll = long long;

Graph annealing(Graph& graph, Params& params ) {
    const int maxT = params.get("maxT");
    const int minT = params.get("minT");
    const int N = params.get("N");
    const int seed = params.get("seed");
    double arpha = pow(double(minT)/maxT, double(1)/N);

    mt19937 mt;
    uniform_real_distribution<double> div(0, 1);

    mt.seed(seed);

    Graph bestgraph = graph;
    graph_info_t best_info = graph.sumD();
    double T = maxT;

    double haspl_prev = graph.calcHaspl(best_info["sumd"]);
    int diam_prev = best_info["diam"];

    for ( int i = 0; i < N; ++i ) {
        cout << endl;
        cout << "i = " << i << endl;
        T = arpha*T;

        pair<Edge,Edge> pee = select_edges3(graph, mt);
        graph.simple_swing(pee.first, pee.second);
        if ( !graph.isLinking() ) {
            cout << "Not linking" << endl;
            graph.back();
            continue;
        }

        graph_info_t g_info = graph.sumD();
        double haspl = graph.calcHaspl(g_info["sumd"]); 
        int diam = g_info["sumd"];

        cout << "haspl = " << haspl << endl;
        cout << "diam = " << g_info["diam"] << endl;

        if ( diam < g_info["diam"] ) {
            best_info = g_info;
            bestgraph = graph;
        }
        if ( g_info["sumd"] < best_info["sumd"] ) {
            best_info = g_info;
            bestgraph = graph;
        }

        double delta = (haspl-haspl_prev)*graph.getSum_s()*(graph.getSum_h()-1);

        if ( delta <= 0 ) {
            haspl_prev = haspl;
            cout << "Move new graph for better haspl" << endl;
            continue; 
        }

        double prob = pow(M_E, -(delta/T));
        double p = div(mt);
        cout << "probability = " << prob << endl;

        if ( p < prob ) {
            haspl_prev = haspl;
            cout << "Move new graph for probability" << endl;
            continue;
        }
        graph.back();
        cout << "Don't move" << endl;
    }
    return bestgraph;
}

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
    // Graph graph = Graph::make(5200, 10000, 10, 1);
    graph.toDot("graph1.dot", graph);

    Graph bestgraph = graph;
    for ( int i = 0; i < 10; ++i ) {
        params.set("seed", grand_mt());
        bestgraph = annealing(graph, params);
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