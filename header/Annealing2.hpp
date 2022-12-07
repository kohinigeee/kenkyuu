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
#include "MDST.hpp"

using namespace std;

vector<bool> annealing_logs(2, true);

Graph annealing(Graph& graph, Params& params, pair<Edge,Edge> (*select)(Graph&, mt19937&) ) {
    const int maxT = params.get("maxT");
    const int minT = params.get("minT");
    const int N = params.get("N");
    const int seed = params.get("seed");
    double arpha = pow(double(minT)/maxT, double(1)/N);

    const bool annealing_log = annealing_logs[0];
    mt19937 mt;
    uniform_real_distribution<double> div(0, 1);

    mt.seed(seed);

    Graph bestgraph = graph;
    graph_info_t best_info = graph.sumD();
    double T = maxT;

    graph_info_t prev_info = best_info;

    for ( int i = 0; i < N; ++i ) {
        if ( annealing_log ) {
        cout << endl;
        cout << "i = " << i << endl;
        }
        T = arpha*T;

        pair<Edge,Edge> pee = select(graph, mt); 
        pee = bias(graph, pee, mt);
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

        if ( diam < best_info["diam"] ) {
            best_info = g_info;
            bestgraph = graph;

        }
        if ( g_info["sumd"] < best_info["sumd"] ) {
            best_info = g_info;
            bestgraph = graph;
        }

        double prob = eval1(graph, prev_info, g_info, T );
        double p = div(mt);

        if ( annealing_log ) cout << "probability = " << prob << endl;
        if ( prob >= 1 ) {
            if ( annealing_log ) cout << "Move to better graph" << endl;
            prev_info = g_info;
            continue;
        }
        if ( p < prob ) {
            if ( annealing_log ) cout << "Move to new graph for probability" << endl;
            prev_info = g_info;
            continue;
        }

        if ( annealing_log ) cout << "Don't move" << endl;
        graph.back();
    }
    return bestgraph;
}

//MDSTによって初期解を再度生成し、annealingを複数回回す手法
// @n: annealingを回す回数(初期解を生成する回数)
Graph annealingWithMDST ( int n, Graph& graph, Params params, pair<Edge,Edge> (*select)(Graph&, mt19937&), mt19937& mt) {
    Graph g = graph;
    Graph best = graph;
    graph_info_t best_info = best.sumD();

    const bool annealing_log = annealing_logs[1];

    for ( int i = 0; i <= n; ++i ) {
        params.set("seed", mt());

        Graph tmp = annealing(g, params, select); 
        graph_info_t tmp_info = tmp.sumD();

        double haspl = tmp.calcHaspl(tmp_info["sumd"]);
        double best_haspl = best.calcHaspl(best_info["sumd"]);
        long long tmp_diam = tmp_info["diam"];
        long long best_diam = best_info["diam"];

        if ( tmp_diam < best_diam ) {
            best = tmp;
            best_info = tmp_info;
        } else if ( tmp_diam == best_diam && haspl < best_haspl ) {
            best = tmp;
            best_info = tmp_info;
        } 
        if ( annealing_log ) {
            cout << "annealing : " << i+1 << endl;
            cout << "[tmp-graph]" << endl;
            cout << "diam = " << tmp_diam << ", haspl = " << haspl << endl;
            cout << "[best-graph]" << endl;
            cout << "diam = " << best_diam << ", haspl = " << best_haspl << endl;
            cout << endl;
        }

        Graph::set_seed(mt());
        g = makeMDTgraph(best); 
        g.linkLoops();
    }
    return best;
}

void annealing_log_on(int no) {
    annealing_logs[no] = true;
}

void annealing_log_off(int no) {
    annealing_logs[no] = false;
}

void annealing_log_on_all() {
    for ( int i = 0; i < annealing_logs.size(); ++i ) annealing_logs[i] = true;
}
    
void annealing_log_off_all() {
    for ( int i = 0; i < annealing_logs.size(); ++i ) annealing_logs[i] = false;
}
#endif