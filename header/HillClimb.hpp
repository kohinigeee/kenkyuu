#ifndef INCLUDE_HILLCLIMB_HPP
#define INCLUDE_HILLCLIMB_HPP

#include "time.h"

#include "Graph.hpp"
#include "Annealing.hpp"
#include "MDST.hpp"
#include "Result.hpp"

//履歴用

using hillclimb_func = Graph(*)(Graph&, const int n, const int seed, select_func);
int hill_limt_eval = -1;

long long hill_limt_time = -1;

void set_hill_limt(int limt ) {
    hill_limt_eval = limt;
}

//hillclimbのマックス時間を指定する(単位:sec)
void set_hill_limt_time(long long timev) {
    hill_limt_time = timev; 
}

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
    return ans*(ans-1)/2;
}

Graph hillclimb( Graph& graph, const double alpha, const int seed, pair<Edge,Edge>(*select)(Graph&,mt19937&), long long start_time = 0, History& history = defo_history ) {
    const long long limn = countEdges(graph)*alpha;
    const time_t st_t = clock();
    
    mt19937 mt;
    mt.seed(seed);
    Graph::set_seed(mt());

    Graph best = graph;
    GraphInfo best_info = graph.calcInfo();

    set<pair<Edge,Edge>> calced; //計算済みペア
    char buff[256];

    long long limcnt = 0;

    long long evalcnt = 0;
    while( limcnt < limn ) {
        
        time_t now_t = clock();
        long long dura_sec = double(now_t-st_t)/CLOCKS_PER_SEC;
        // cout << "[Log] hillclimb:: sepnt time = " << dura_sec << endl;

        if ( hill_limt_time > 0 ) {
            if ( start_time+dura_sec > hill_limt_time ) return best;
        }

        if ( hill_limt_eval > 0 && hill_limt_eval <= evalcnt ) break;

        pair<Edge,Edge> pee = select(graph, mt);
        
        pair<Edge,Edge> tmp = makeEdgePair(pee.first, pee.second);
        if ( calced.find(tmp) != calced.end() ) {
            limcnt++;
            continue;
        }

        calced.insert(tmp);

        graph.simple_swing(pee.first, pee.second);

        ++evalcnt;

        if ( !graph.isLinking() ) {
            graph.back();
            ++limcnt;
            continue;
        }

        GraphInfo new_info = graph.calcInfo();
        
        if ( compInfo(new_info, best_info) ) {

        // sprintf(buff, "[Log] s=%d, h=%d, r=%d : %d, %.5lf\n", graph.getSum_s(), graph.getSum_h(), graph.get_r(), new_info.get_diam(), new_info.get_haspl());
        // cout << buff << flush;

            best_info = new_info;
            best = graph;
            limcnt = 0;
            
            history.add(make_pair(new_info.get_diam(), new_info.get_haspl()));
        } else {
            graph.back();
            ++limcnt;
            
            // history.add(make_pair(new_info.get_diam(), new_info.get_haspl()));
            history.add(history.values.back());
        }
    }
    return best;
}

// //limcnt : 連続キック限界数(未更新が続く最大回数)
// Graph hillClimbing_normalkick(Graph& graph, int limt, const double alpha, const int seed,  pair<Edge,Edge>(*select)(Graph&, mt19937&) ) {
//     mt19937 mt;
//     mt.seed(seed);
//     const int s = graph.getSum_s(), h = graph.getSum_h(), r = graph.get_r();

//     Graph best = graph;
//     GraphInfo best_info = graph.calcInfo();
//     int limcnt = 0;
//     do {
//         Graph tmp = hillclimb(graph, alpha, mt(), select);
//         GraphInfo new_info = tmp.calcInfo();

//         if ( compInfo(new_info, best_info) ) {
//             best= tmp;
//             best_info = new_info;
//             limcnt = 0;
//         }
//         graph = Graph::make(s, h, r, 1);
//         ++limcnt;
//     } while( limcnt <= limt );
//     return best;
// }

// //limcnt : 連続キック限界数(未更新が続く最大回数)
// Graph hillClimbing_mdstkick(Graph& graph, int limt, const double alpha, const int seed,  pair<Edge,Edge>(*select)(Graph&, mt19937&) ) {
//     mt19937 mt;
//     mt.seed(seed);
//     const int s = graph.getSum_s(), h = graph.getSum_h(), r = graph.get_r();

//     Graph best = graph;
//     GraphInfo best_info = graph.calcInfo();
//     int limcnt = 0;
//     do {
//         Graph tmp = hillclimb(graph, alpha, mt(), select);
//         GraphInfo new_info = tmp.calcInfo();

//         if ( compInfo(new_info, best_info) ) {
//             best= tmp;
//             best_info = new_info;
//             limcnt = 0;
//         }
//         graph = makeMDTgraph(best);
//         graph.linkLoops();
//         ++limcnt;
//     } while( limcnt <= limt );
//     return best;
// }

// Graph hillClimbing_mdstkick(Graph& graph, const int n, const int seed,  pair<Edge,Edge>(*select)(Graph&, mt19937&) ) {

//     const int limn = n*0.2;

//     mt19937 mt;
//     mt.seed(seed);
//     Graph::set_seed(mt());

//     Graph best = graph;
//     graph_info_t best_info = graph.sumD();
//     double best_haspl = graph.calcHaspl(best_info["sumd"]);

//     graph_info_t prev_info = best_info;
//     double prev_haspl = best_haspl;

//     int limcnt = 0;
//     for ( int i = 0; i < n; ++i ) {

//         if ( limcnt >= limn ) {
//             Graph::set_seed(mt());
//             graph = makeMDTgraph(graph);
//             graph.linkLoops();
//             limcnt = 0; 
//         }

//         pair<Edge,Edge> pee =select(graph, mt);
//         //bias処理 なし
//         graph.simple_swing(pee.first, pee.second);
//         if ( !graph.isLinking() ) {
//             graph.back();
//             ++limcnt;
//             continue;
//         } 

//         graph_info_t tmp_info = graph.sumD();
//         double tmp_haspl = graph.calcHaspl(tmp_info["sumd"]);
//         int tmp_diam = tmp_info["diam"];
//         pair<long long, double> tmp_pair = make_pair(tmp_diam, tmp_haspl);

//         if ( eval_graph(tmp_pair, make_pair(best_info["diam"], best_haspl ) ) ) {
//             best = graph;
//             best_info = tmp_info;
//             best_haspl = tmp_haspl;
//         }

//         if ( eval_graph(tmp_pair, make_pair(prev_info["diam"], prev_haspl)) ) {
//             //近傍移動成功
//             prev_info = tmp_info;
//             prev_haspl = tmp_haspl;
//             limcnt = 0;

//             // cout << "i = " << i << endl;
//             // cout << "[ move better graph]" << endl;
//             // cout << " diam = " << tmp_info["diam"] << endl;
//             // cout << " haspl = " << tmp_haspl << endl;
//         } else {
//             //近傍移動失敗
//             graph.back();
//             ++limcnt;
//         }
//     }

//     return best;
// }

// Graph dividehillClimbing_normalkick(Graph& graph, const int n, const int seed,  pair<Edge,Edge>(*select)(Graph&, mt19937&), const int reserved_port ) {

//     const int limn = n*0.2;
//     const int s = graph.getSum_s(), h = graph.getSum_h(), r = graph.get_r();

//     mt19937 mt;
//     Graph::set_seed(mt());

//     Graph best = graph;
//     graph_info_t best_info = graph.sumD();
//     double best_haspl = graph.calcHaspl(best_info["sumd"]);

//     graph_info_t prev_info = best_info;
//     double prev_haspl = best_haspl;

//     int limcnt = 0;
//     for ( int i = 0; i < n; ++i ) {

//         if ( limcnt >= limn ) {
//             Graph::set_seed(mt());
//             graph = Graph::make3(s, h, r, 1, reserved_port);
//             limcnt = 0; 
//         }

//         pair<Edge,Edge> pee =select(graph, mt);
//         //bias処理 なし
//         graph.simple_swing(pee.first, pee.second);
//         if ( !graph.isLinking() ) {
//             graph.back();
//             ++limcnt;
//             continue;
//         } 

//         graph_info_t tmp_info = graph.sumD();
//         double tmp_haspl = graph.calcHaspl(tmp_info["sumd"]);
//         int tmp_diam = tmp_info["diam"];
//         pair<long long, double> tmp_pair = make_pair(tmp_diam, tmp_haspl);

//         if ( eval_graph(tmp_pair, make_pair(best_info["diam"], best_haspl ) ) ) {
//             best = graph;
//             best_info = tmp_info;
//             best_haspl = tmp_haspl;
//         }

//         if ( eval_graph(tmp_pair, make_pair(prev_info["diam"], prev_haspl)) ) {
//             //近傍移動成功
//             prev_info = tmp_info;
//             prev_haspl = tmp_haspl;
//             limcnt = 0;

//             // cout << "i = " << i << endl;
//             // cout << "[ move better graph]" << endl;
//             // cout << " diam = " << tmp_info["diam"] << endl;
//             // cout << " haspl = " << tmp_haspl << endl;
//         } else {
//             //近傍移動失敗
//             graph.back();
//             ++limcnt;
//         }
//     }

//     return best;
// }

// Graph dividehillClimbing_mdstkick(Graph& graph, const int n, const int seed,  pair<Edge,Edge>(*select)(Graph&, mt19937&), const int reserve_port ) {

//     const int limn = n*0.2;

//     mt19937 mt;
//     Graph::set_seed(mt());

//     Graph best = graph;
//     graph_info_t best_info = graph.sumD();
//     double best_haspl = graph.calcHaspl(best_info["sumd"]);

//     graph_info_t prev_info = best_info;
//     double prev_haspl = best_haspl;

//     int limcnt = 0;
//     for ( int i = 0; i < n; ++i ) {

//         if ( limcnt >= limn ) {
//             Graph::set_seed(mt());
//             graph = makeMDTgraph(graph);
//             graph.linkLoops();
//             limcnt = 0; 
//         }

//         pair<Edge,Edge> pee =select(graph, mt);
//         //bias処理 なし
//         graph.simple_swing(pee.first, pee.second);
//         if ( !graph.isLinking() ) {
//             graph.back();
//             ++limcnt;
//             continue;
//         } 

//         graph_info_t tmp_info = graph.sumD();
//         double tmp_haspl = graph.calcHaspl(tmp_info["sumd"]);
//         int tmp_diam = tmp_info["diam"];
//         pair<long long, double> tmp_pair = make_pair(tmp_diam, tmp_haspl);

//         if ( eval_graph(tmp_pair, make_pair(best_info["diam"], best_haspl ) ) ) {
//             best = graph;
//             best_info = tmp_info;
//             best_haspl = tmp_haspl;
//         }

//         if ( eval_graph(tmp_pair, make_pair(prev_info["diam"], prev_haspl)) ) {
//             //近傍移動成功
//             prev_info = tmp_info;
//             prev_haspl = tmp_haspl;
//             limcnt = 0;

//             cout << "i = " << i << endl;
//             cout << "[ move better graph]" << endl;
//             cout << " diam = " << tmp_info["diam"] << endl;
//             cout << " haspl = " << tmp_haspl << endl;
//         } else {
//             //近傍移動失敗
//             graph.back();
//             ++limcnt;
//         }
//     }

//     return best;
// }

#endif