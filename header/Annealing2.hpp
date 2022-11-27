#ifndef INCLUDE_ANNEALING2_HPP
#define INCLUDE_ANNEALING2_HPP

#include <random>
#include <utility>
#include <cmath>
#include <map>
#include <string>
#include <algorithm>
#include "Graph.hpp"
#include "Annealing.hpp"

using namespace std;

bool annealing_log = true;

Graph annealing(Graph& graph, Params& params, pair<Edge,Edge> (*select)(Graph&, mt19937&) ) {
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
        if ( annealing_log ) {
        cout << endl;
        cout << "i = " << i << endl;
        }
        T = arpha*T;

        pair<Edge,Edge> pee = select(graph, mt); 
        graph.simple_swing(pee.first, pee.second);
        if ( !graph.isLinking() ) {
            if ( annealing_log ) cout << "Not linking" << endl;
            graph.back();
            continue;
        }

        graph_info_t g_info = graph.sumD();
        double haspl = graph.calcHaspl(g_info["sumd"]); 
        int diam = g_info["sumd"];

        if ( annealing_log ) { 
        cout << "haspl = " << haspl << endl;
        cout << "diam = " << g_info["diam"] << endl;
        }

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
            if ( annealing_log ) cout << "Move new graph for better haspl" << endl;
            continue; 
        }

        double prob = pow(M_E, -(delta/T));
        double p = div(mt);
        if ( annealing_log ) cout << "probability = " << prob << endl;

        if ( p < prob ) {
            haspl_prev = haspl;
            if ( annealing_log ) cout << "Move new graph for probability" << endl;
            continue;
        }
        graph.back();
        if ( annealing_log ) cout << "Don't move" << endl;
    }
    return bestgraph;
}

//MDSTによって初期解を再度生成し、annealingを複数回回す手法
// @n: annealingを回す回数(初期解を生成する回数)
Graph annealingWithMDST ( int n, Graph& graph, Params params, pair<Edge,Edge> (*select)(Graph&, mt19937&), mt19937& mt) {
    Graph g = graph;
}
void annealing_log_on() {
    annealing_log = true;
}

void annealing_log_off() {
    annealing_log = false;
}
#endif