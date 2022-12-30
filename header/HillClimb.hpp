#ifndef INCLUDE_HILLCLIMB_HPP
#define INCLUDE_HILLCLIMB_HPP

#include "Graph.hpp"
#include "Annealing.hpp"
#include "MDST.hpp"

using hillclimb_func = Graph(*)(Graph&, const int n, const int seed, select_func);

long long countEdges(Graph& graph) {
    long long ans = 0;
    for ( int i = 0; i < graph.getSum_s(); ++i ) {
        G_no g_no = G_no(i/graph.get_us());
        Node_no node_no = Node_no(i%graph.get_us());

        const Switch& sw = graph.getSwitch(g_no, node_no);
        for ( const Edge& e : sw.getEdges() ) {
            if ( e.getType() == Edge::edgeType::NONE ) continue;
            if ( e.getStatus() == Edge::edgeStatus::LOCKED ) continue;

            if ( e.getType() == Edge::edgeType::SWITCH ) {
                int to_g = e.getG().getNo();
                int to_node = e.getNode().getNo();

                if ( to_g > g_no.getNo() ) ++ans;
                if ( to_g == g_no.getNo() && to_node > node_no.getNo() ) ++ans;
            } else {
                ++ans;
            }
        } 
    }
    return ans;
}

Graph hillClimbing_normalkick(Graph& graph, const int n, const int seed,  pair<Edge,Edge>(*select)(Graph&, mt19937&) ) {

    const int limn = n*0.2;
    const int s = graph.getSum_s(), h = graph.getSum_h(), r = graph.get_r();

    mt19937 mt;
    mt.seed(seed);
    Graph::set_seed(mt());

    Graph best = graph;
    graph_info_t best_info = graph.sumD();
    double best_haspl = graph.calcHaspl(best_info["sumd"]);

    graph_info_t prev_info = best_info;
    double prev_haspl = best_haspl;

    int limcnt = 0;
    for ( int i = 0; i < n; ++i ) {

        if ( limcnt >= limn ) {
            Graph::set_seed(mt());
            graph = Graph::make(s, h, r, 1);
            limcnt = 0; 
        }

        pair<Edge,Edge> pee =select(graph, mt);
        //bias処理 なし
        graph.simple_swing(pee.first, pee.second);
        if ( !graph.isLinking() ) {
            graph.back();
            ++limcnt;
            continue;
        } 

        graph_info_t tmp_info = graph.sumD();
        double tmp_haspl = graph.calcHaspl(tmp_info["sumd"]);
        int tmp_diam = tmp_info["diam"];
        pair<long long, double> tmp_pair = make_pair(tmp_diam, tmp_haspl);

        if ( eval_graph(tmp_pair, make_pair(best_info["diam"], best_haspl ) ) ) {
            best = graph;
            best_info = tmp_info;
            best_haspl = tmp_haspl;
        }

        if ( eval_graph(tmp_pair, make_pair(prev_info["diam"], prev_haspl)) ) {
            //近傍移動成功
            prev_info = tmp_info;
            prev_haspl = tmp_haspl;
            limcnt = 0;

            // cout << "i = " << i << endl;
            // cout << "[ move better graph]" << endl;
            // cout << " diam = " << tmp_info["diam"] << endl;
            // cout << " haspl = " << tmp_haspl << endl;
        } else {
            //近傍移動失敗
            graph.back();
            ++limcnt;
        }
    }

    return best;
}

Graph hillClimbing_mdstkick(Graph& graph, const int n, const int seed,  pair<Edge,Edge>(*select)(Graph&, mt19937&) ) {

    const int limn = n*0.2;

    mt19937 mt;
    mt.seed(seed);
    Graph::set_seed(mt());

    Graph best = graph;
    graph_info_t best_info = graph.sumD();
    double best_haspl = graph.calcHaspl(best_info["sumd"]);

    graph_info_t prev_info = best_info;
    double prev_haspl = best_haspl;

    int limcnt = 0;
    for ( int i = 0; i < n; ++i ) {

        if ( limcnt >= limn ) {
            Graph::set_seed(mt());
            graph = makeMDTgraph(graph);
            graph.linkLoops();
            limcnt = 0; 
        }

        pair<Edge,Edge> pee =select(graph, mt);
        //bias処理 なし
        graph.simple_swing(pee.first, pee.second);
        if ( !graph.isLinking() ) {
            graph.back();
            ++limcnt;
            continue;
        } 

        graph_info_t tmp_info = graph.sumD();
        double tmp_haspl = graph.calcHaspl(tmp_info["sumd"]);
        int tmp_diam = tmp_info["diam"];
        pair<long long, double> tmp_pair = make_pair(tmp_diam, tmp_haspl);

        if ( eval_graph(tmp_pair, make_pair(best_info["diam"], best_haspl ) ) ) {
            best = graph;
            best_info = tmp_info;
            best_haspl = tmp_haspl;
        }

        if ( eval_graph(tmp_pair, make_pair(prev_info["diam"], prev_haspl)) ) {
            //近傍移動成功
            prev_info = tmp_info;
            prev_haspl = tmp_haspl;
            limcnt = 0;

            // cout << "i = " << i << endl;
            // cout << "[ move better graph]" << endl;
            // cout << " diam = " << tmp_info["diam"] << endl;
            // cout << " haspl = " << tmp_haspl << endl;
        } else {
            //近傍移動失敗
            graph.back();
            ++limcnt;
        }
    }

    return best;
}

Graph dividehillClimbing_normalkick(Graph& graph, const int n, const int seed,  pair<Edge,Edge>(*select)(Graph&, mt19937&), const int reserved_port ) {

    const int limn = n*0.2;
    const int s = graph.getSum_s(), h = graph.getSum_h(), r = graph.get_r();

    mt19937 mt;
    Graph::set_seed(mt());

    Graph best = graph;
    graph_info_t best_info = graph.sumD();
    double best_haspl = graph.calcHaspl(best_info["sumd"]);

    graph_info_t prev_info = best_info;
    double prev_haspl = best_haspl;

    int limcnt = 0;
    for ( int i = 0; i < n; ++i ) {

        if ( limcnt >= limn ) {
            Graph::set_seed(mt());
            graph = Graph::make3(s, h, r, 1, reserved_port);
            limcnt = 0; 
        }

        pair<Edge,Edge> pee =select(graph, mt);
        //bias処理 なし
        graph.simple_swing(pee.first, pee.second);
        if ( !graph.isLinking() ) {
            graph.back();
            ++limcnt;
            continue;
        } 

        graph_info_t tmp_info = graph.sumD();
        double tmp_haspl = graph.calcHaspl(tmp_info["sumd"]);
        int tmp_diam = tmp_info["diam"];
        pair<long long, double> tmp_pair = make_pair(tmp_diam, tmp_haspl);

        if ( eval_graph(tmp_pair, make_pair(best_info["diam"], best_haspl ) ) ) {
            best = graph;
            best_info = tmp_info;
            best_haspl = tmp_haspl;
        }

        if ( eval_graph(tmp_pair, make_pair(prev_info["diam"], prev_haspl)) ) {
            //近傍移動成功
            prev_info = tmp_info;
            prev_haspl = tmp_haspl;
            limcnt = 0;

            // cout << "i = " << i << endl;
            // cout << "[ move better graph]" << endl;
            // cout << " diam = " << tmp_info["diam"] << endl;
            // cout << " haspl = " << tmp_haspl << endl;
        } else {
            //近傍移動失敗
            graph.back();
            ++limcnt;
        }
    }

    return best;
}

Graph dividehillClimbing_mdstkick(Graph& graph, const int n, const int seed,  pair<Edge,Edge>(*select)(Graph&, mt19937&), const int reserve_port ) {

    const int limn = n*0.2;

    mt19937 mt;
    Graph::set_seed(mt());

    Graph best = graph;
    graph_info_t best_info = graph.sumD();
    double best_haspl = graph.calcHaspl(best_info["sumd"]);

    graph_info_t prev_info = best_info;
    double prev_haspl = best_haspl;

    int limcnt = 0;
    for ( int i = 0; i < n; ++i ) {

        if ( limcnt >= limn ) {
            Graph::set_seed(mt());
            graph = makeMDTgraph(graph);
            graph.linkLoops();
            limcnt = 0; 
        }

        pair<Edge,Edge> pee =select(graph, mt);
        //bias処理 なし
        graph.simple_swing(pee.first, pee.second);
        if ( !graph.isLinking() ) {
            graph.back();
            ++limcnt;
            continue;
        } 

        graph_info_t tmp_info = graph.sumD();
        double tmp_haspl = graph.calcHaspl(tmp_info["sumd"]);
        int tmp_diam = tmp_info["diam"];
        pair<long long, double> tmp_pair = make_pair(tmp_diam, tmp_haspl);

        if ( eval_graph(tmp_pair, make_pair(best_info["diam"], best_haspl ) ) ) {
            best = graph;
            best_info = tmp_info;
            best_haspl = tmp_haspl;
        }

        if ( eval_graph(tmp_pair, make_pair(prev_info["diam"], prev_haspl)) ) {
            //近傍移動成功
            prev_info = tmp_info;
            prev_haspl = tmp_haspl;
            limcnt = 0;

            cout << "i = " << i << endl;
            cout << "[ move better graph]" << endl;
            cout << " diam = " << tmp_info["diam"] << endl;
            cout << " haspl = " << tmp_haspl << endl;
        } else {
            //近傍移動失敗
            graph.back();
            ++limcnt;
        }
    }

    return best;
}

#endif