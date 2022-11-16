//random選択　(ホスト辺考慮あり)
#include<iostream>
#include<vector>
#include<cmath>
#include<random>
#include<string>

#include "./header/Debug.hpp"
#include "./header/functions.hpp"
#include "./header/Graph.hpp"
#include "./header/Annealing.hpp"

using namespace std;
using ll = long long;

int main(int argc, char** args)
{
    debug_off();

    vector<string> arg;
    for ( int i = 1; i < argc; ++i ) arg.push_back(args[i]);

    Params params = Params(arg);
    const int maxT = params.get("maxT");
    const int minT = params.get("minT");
    const int N = params.get("N");
    const int seed = params.get("seed");
    double arpha = pow(double(minT)/maxT, double(1)/N);
    
    mt19937 mt;
    uniform_real_distribution<double> div(0, 1);

    mt.seed(seed);

    try {
    Graph graph = Graph::make(16, 32, 4, 1);
    graph.toDot("graph1.dot", graph); 

    double T = maxT;

    graph_info_t g_info = graph.sumD();
    double haspl_prev = (double)g_info["sumd"]/(graph.getSum_h()*(graph.getSum_h()-1)/2); 

    double min_diam = g_info["diam"];
    double min_haspl = haspl_prev;
    Graph bestgraph = graph;

    for ( int i = 0; i < N; ++i ) {
        cout << endl;
        cout << "i = " << i+1 << endl;
        T = arpha*T;
        pair<Edge, Edge> pee = select_edges2(graph, mt);
        graph.simple_swing(pee.first, pee.second);
        if ( !graph.isLinking() ) {
            graph.back();
            continue;
        }

        graph_info_t ginfo = graph.sumD();

        double haspl = (double)ginfo["sumd"]/(graph.getSum_h()*(graph.getSum_h()-1)/2);

        cout << "haspl_prev = " << haspl_prev << endl;
        cout << "haspl = " << haspl << endl;

        if ( ginfo["diam"] < min_diam ) {
            min_diam = ginfo["diam"];
            min_haspl = haspl;
            bestgraph = graph;
        }
        if ( ginfo["diam"] == min_diam && haspl < min_haspl ) {
            min_diam = ginfo["diam"];
            min_haspl = haspl;
            bestgraph = graph;
        } 

        double delta = (haspl-haspl_prev)*graph.getSum_s()*(graph.getSum_h()-1);
        if ( delta < 0 ) {
            haspl_prev = haspl;
            continue;
        }

        double prob = pow(M_E, -(delta/T));
        double p = div(mt);

        if ( p < prob ) {
            haspl_prev = haspl;
            continue;
        }
        graph.back();
    }

    Graph::toDot("graph2.dot", bestgraph);
    cout << endl;

    cout << "[Paramaters]" << endl;
    cout << "minT: " << minT << " maxT: " << maxT << endl;
    cout << "N: " << N << endl;
    cout << "seed: " << seed << endl;
    cout << "\n[Results]" << endl;
    cout << "min_diam = " << min_diam << endl;
    cout << "min_hasple = " << min_haspl << endl;
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }

}