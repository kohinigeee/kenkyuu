#ifndef INCLUDE_DIRECTHILLCLIMB_HPP
#define INCLUDE_DIRECTHILLCLIMB_HPP

//指向性をもったエッジ選択
#include<set>
#include<algorithm>

#include "Graph.hpp"
#include "MDST.hpp"
#include "HillClimb.hpp"

using namespace std;
using directHillclimbMethod_t = bool(*) (Graph& graph, long long st_time, long long limt_time);

Edge takeHostEdge(Graph& graph, G_no g_no, Node_no node_no) {
    Switch sw = graph.getSwitch(g_no, node_no);
    if ( sw.get_hsize() <= 0 ) {
        cout << "[Error] takeHostEdge:: this switch has no host" << endl;
        exit(1);
    }

    Edge ans;
    for ( Edge e : sw.getEdges() ) {
        if ( e.getType() != Edge::edgeType::HOST ) continue;
        ans = e;
        break;
    }
    return ans;
} 

//PathTreeを用いて辺の評価
map<pair<Edge,Edge>, long long> makeEdgeScoresWithPathTrees( Graph& graph, vector<PathTree>& trees ) {
    map<pair<Edge,Edge>, long long> scores;

    for ( PathTree& tree : trees ) {
        for ( int level = 0; level <= tree.get_max_level(); ++level ) {
            for ( TreeNode& node : tree.get_tnodes(level) ) {
                for ( Edge& e : node.belows ) {
                    long long tmpscore = (tree.get_max_level()-node.level);
                    scores[makeEdgePair(e, graph.getEdge(e))] -= tmpscore;
                }
                for ( Edge& e : node.flats ) {
                    long long tmpscore = node.level;
                    scores[makeEdgePair(e, graph.getEdge(e))] += tmpscore;
                }
            }
        }
    }
    return scores;
}

//種数1前提
//戻り値: 更新が行われたか
//edge_scores: makeEdgeScoreWithPathTree
bool directHillclimb_once_2(Graph& graph, long long st_time = 0, long long limt_time = -1 ) {

    GraphResult result = graph.calcBFS();
    GraphInfo info = graph.calcInfo(result);
    bool isUpdated = false;
    time_t time_start = clock();

    while(1) {
        bool isContinue = false;
        set<pair<Edge,Edge>> isCalced; //計算済みのエッジペアを保存

        vector<pair<int, int>> pairs = info.get_pairs();

        sort(pairs.begin(), pairs.end(), [&graph](pair<int,int> p1, pair<int,int> p2) {
            int h1_1 = graph.getSwitch(G_no(0), Node_no(p1.first)).get_hsize();
            int h1_2 = graph.getSwitch(G_no(0), Node_no(p1.second)).get_hsize();
            
            int h2_1 = graph.getSwitch(G_no(0), Node_no(p2.first)).get_hsize();
            int h2_2 = graph.getSwitch(G_no(0), Node_no(p2.second)).get_hsize();

            return (h1_1)*(h1_2) > (h2_1)*(h2_2);
        }); 
        
        for ( int i = 0; i < info.get_pairs().size(); ++i ) {
            
            for ( int _ = 0; _ < 2; ++_ ) { 
            int v1 = pairs[i].first, v2 = pairs[i].second;

            if ( _ == 1 ) swap(v1,v2);
            path_t diamPath = graph.takePath(v1, v2);

            set<pair<Edge,Edge>> pathset; //エッジのパス内存在判定の際に用いるset
            for ( auto p : diamPath ) {
                pair<Edge,Edge> tmp = makeEdgePair(p.first, p.second);
                pathset.insert(tmp);
            }

            //交換候補となるホスト辺
            Edge hostEdge = takeHostEdge(graph, diamPath[0].second.getG(), diamPath[0].second.getNode());

            //交換候補となるエッジを取り出す()
            vector<Node_no> nodes;
            vector<Edge> edges;
            for ( int i = 1; i < diamPath.size()-1; ++i ) {
                nodes.push_back(diamPath[i].first.getNode());
            }


            for ( Node_no node_no : nodes ) {
                Switch sw = graph.getSwitch(G_no(0), node_no);

                for ( Edge e : sw.getEdges() ) {
                    if ( e.getType() != Edge::edgeType::SWITCH ) continue;
                    if ( e.getStatus() == Edge::edgeStatus::LOCKED ) continue;

                    Edge oppo_edge = graph.getEdge(e);
                    if ( pathset.find(makeEdgePair(e, oppo_edge)) != pathset.end() ) continue;
                    edges.push_back(e); 
                }
            }

            //候補エッジの並び替え
            map<pair<Edge,Edge>, long long> edgeScores = makeEdgeScoresWithPathTrees(graph, result.getTrees());
            sort(edges.begin(), edges.end(), [&graph, &edgeScores](const Edge& e1, const Edge& e2) {
                pair<Edge,Edge> pe1 = makeEdgePair(e1, graph.getEdge(e1)),
                                pe2 = makeEdgePair(e2, graph.getEdge(e2));
                return edgeScores[pe1] > edgeScores[pe2];
            });
        
            //候補エッジによる近傍操作
            //即時移動戦略

            for ( Edge swEdge : edges ) {
                time_t now = clock();
                long long dura = double(now-time_start)/CLOCKS_PER_SEC;
                if( limt_time > 0 && dura+st_time >= limt_time ) return false;

                Graph tmp = graph;

                pair<Edge,Edge> pe = makeEdgePair(swEdge, hostEdge);
                if ( isCalced.find(pe) != isCalced.end() ) continue;
                isCalced.insert(pe);

                tmp.simple_swing(swEdge, hostEdge);
                //不連結は除外
                if ( !tmp.isLinking() ) continue;

                GraphResult new_result = tmp.calcBFS();
                GraphInfo new_info = tmp.calcInfo(new_result);

                pair<long long, double> p1 = make_pair(info.get_diam(), info.get_haspl()),
                                        p2 = make_pair(new_info.get_diam(), new_info.get_haspl());
            
                if ( p2 < p1 ) {
                    // cout << "[Log] new: " << new_info.get_diam() << ", " << new_info.get_haspl() << endl;
                    result = new_result;
                    info = new_info;
                    graph = tmp;
                    isUpdated = true;
                    isContinue = true;
                    break;
                }
            }
            if ( isContinue ) break;
            }
            if ( isContinue ) break;
        }
        if ( isContinue ) continue;
        break;
    }
    return isUpdated;
}

//種数1前提
//戻り値: 更新が行われたか
bool directHillclimb_once(Graph& graph, long long st_time = 0, long long limt_time = -1) {

    GraphResult result = graph.calcBFS();
    GraphInfo info = graph.calcInfo(result);
    bool isUpdated = false;

    time_t time_start = clock();

    while(1) {
        bool isContinue = false;
        set<pair<Edge,Edge>> isCalced; //計算済みのエッジペアを保存

        vector<pair<int, int>> pairs = info.get_pairs();

        sort(pairs.begin(), pairs.end(), [&graph](pair<int,int> p1, pair<int,int> p2) {
            int h1_1 = graph.getSwitch(G_no(0), Node_no(p1.first)).get_hsize();
            int h1_2 = graph.getSwitch(G_no(0), Node_no(p1.second)).get_hsize();
            
            int h2_1 = graph.getSwitch(G_no(0), Node_no(p2.first)).get_hsize();
            int h2_2 = graph.getSwitch(G_no(0), Node_no(p2.second)).get_hsize();

            return (h1_1)*(h1_2) > (h2_1)*(h2_2);
        }); 
        
        for ( int i = 0; i < info.get_pairs().size(); ++i ) {
            
            for ( int _ = 0; _ < 2; ++_ ) { 
            int v1 = pairs[i].first, v2 = pairs[i].second;

            if ( _ == 1 ) swap(v1,v2);
            path_t diamPath = graph.takePath(v1, v2);

            set<pair<Edge,Edge>> pathset; //エッジのパス内存在判定の際に用いるset
            for ( auto p : diamPath ) {
                pair<Edge,Edge> tmp = makeEdgePair(p.first, p.second);
                pathset.insert(tmp);
            }

            //交換候補となるホスト辺
            Edge hostEdge = takeHostEdge(graph, diamPath[0].second.getG(), diamPath[0].second.getNode());

            //交換候補となるエッジを取り出す()
            vector<Node_no> nodes;
            vector<Edge> edges;
            for ( int i = 1; i < diamPath.size()-1; ++i ) nodes.push_back(diamPath[i].first.getNode());


            for ( Node_no node_no : nodes ) {
                Switch sw = graph.getSwitch(G_no(0), node_no);

                for ( Edge e : sw.getEdges() ) {
                    if ( e.getType() != Edge::edgeType::SWITCH ) continue;
                    if ( e.getStatus() == Edge::edgeStatus::LOCKED ) continue;

                    Edge oppo_edge = graph.getEdge(e);
                    if ( pathset.find(makeEdgePair(e, oppo_edge)) != pathset.end() ) continue;
                    edges.push_back(e); 
                }
            }

            //候補エッジの並び替え
            map<pair<Edge,Edge>, long long> edgeScores = result.getEdgeScores();
            sort(edges.begin(), edges.end(), [&graph, &edgeScores](const Edge& e1, const Edge& e2) {
                pair<Edge,Edge> pe1 = makeEdgePair(e1, graph.getEdge(e1)),
                                pe2 = makeEdgePair(e2, graph.getEdge(e2));
                return edgeScores[pe1] < edgeScores[pe2];
            });
        
            //候補エッジによる近傍操作
            //即時移動戦略

            for ( Edge swEdge : edges ) {

                time_t now = clock();
                long long dura = double(now-time_start)/CLOCKS_PER_SEC;
                if( limt_time > 0 && dura+st_time >= limt_time ) return false;

                Graph tmp = graph;

                pair<Edge,Edge> pe = makeEdgePair(swEdge, hostEdge);
                if ( isCalced.find(pe) != isCalced.end() ) continue;
                isCalced.insert(pe);

                tmp.simple_swing(swEdge, hostEdge);
                //不連結は除外
                if ( !tmp.isLinking() ) continue;

                GraphResult new_result = tmp.calcBFS();
                GraphInfo new_info = tmp.calcInfo(new_result);

                pair<long long, double> p1 = make_pair(info.get_diam(), info.get_haspl()),
                                        p2 = make_pair(new_info.get_diam(), new_info.get_haspl());
            
                if ( p2 < p1 ) {
                    // char buff[256];
                    // sprintf(buff, "DirectOnce1::[Log] s=%d, h=%d, r=%d : new=%d, %.5lf  prev=%d, %.5lf\n", graph.getSum_s(), graph.getSum_h(), graph.get_r(), new_info.get_diam(), new_info.get_haspl(), info.get_diam(), info.get_haspl());
                    // cout << buff << flush;

                    result = new_result;
                    info = new_info;
                    graph = tmp;
                    isUpdated = true;
                    isContinue = true;

                    break;
                }
            }
            if ( isContinue ) break;
            }
            if ( isContinue ) break;
        }
        if ( isContinue ) continue;
        break;
    }
    return isUpdated;
}

//指向性山登り+ランダム選択山登り
Graph directHillclimb( Graph& graph_given, mt19937& mt, double alpha, directHillclimbMethod_t dmethod, long long limt_time = -1, long long st_time = 0 ) {
    Graph graph = graph_given;
    set<pair<Edge,Edge>> iscalced;
    int cnt = 0;
    long long spnet_time = 0;
    time_t time_start;

    while(1) {
        time_start = clock();
        if ( dmethod(graph, spnet_time, limt_time) ) continue;
        time_t time_now = clock();
        long long dura = double(time_now-time_start)/CLOCKS_PER_SEC;
        spnet_time += dura;
 
        long long limt = alpha*countEdges(graph); 
        GraphInfo info = graph.calcInfo();
        long long cnt = 0;
        while( cnt < limt ) {
            
            time_t time_now = clock();
            long long dura = double(time_now-time_start)/CLOCKS_PER_SEC;
            spnet_time += dura;
            if ( limt_time > 0 && spnet_time+st_time >= limt_time ){
                return graph;
            }

            // cout << "cnt = " << cnt << endl;
            pair<Edge,Edge> pe = select_edges_noraml(graph, mt); 

            if ( iscalced.find(pe) != iscalced.end() ) {
                ++cnt;
                continue;
            }
            iscalced.insert(pe);

            graph.simple_swing(pe.first, pe.second);
            if ( !(graph.isLinking() ) ) {
                ++cnt;
                graph.back();
                continue;
            } 

            GraphInfo new_info = graph.calcInfo();
            if ( compInfo(new_info, info) ) {
                break;
            } else {
                graph.back();
                ++cnt;
            }
        }

        // GraphInfo new_info = graph.calcInfo();
        // cout << new_info.get_diam() << endl;
        // cout << new_info.get_haspl() << endl;
        // cout << endl;
        if ( cnt >= limt ) break;
    }
    return graph;
}

//dmethod : 指向性山登りのメソッド ( direct_hilcllimb_once or ~_once2 )
//limcnt : 未更新回数の限界値
Graph directHillclimbWithKick( Graph& graph_given, mt19937& mt, int limcnt, directHillclimbMethod_t dmethod, long long limt_time = -1, long long st_time = 0 ) {
    Graph best_graph = graph_given;
    GraphInfo best_info = best_graph.calcInfo();
    int cnt = 0;
    double alpha = 0.8;
    Graph graph = best_graph;

    const int s = graph_given.getSum_s();
    const int h = graph_given.getSum_h();
    const int r = graph_given.get_r();

    long long spent_time = 0;

    while( cnt < limcnt ) {
        
        time_t time_start = clock();
        Graph tmp = directHillclimb(graph, mt, alpha, dmethod, limcnt, spent_time);
        time_t time_now = clock();
        long long dura = double(time_now-time_start)/CLOCKS_PER_SEC;
        spent_time += dura;
        if ( limt_time > 0 && spent_time + st_time >= limt_time ) return best_graph;
        // cout << "[Log] spent time = " << spent_time << endl;

        GraphInfo new_info = tmp.calcInfo();
        
        // cout << "[Log] cnt = "<< cnt << ", new : = " << new_info.get_diam() << ", " << new_info.get_haspl() << endl;
        if ( compInfo(new_info, best_info) ) {
            char buff[256]; 
            sprintf(buff, "DirectOnce1::[Log] s=%d, h=%d, r=%d : new=%d, %.5lf  prev=%d, %.5lf\n", graph.getSum_s(), graph.getSum_h(), graph.get_r(), new_info.get_diam(), new_info.get_haspl(), best_info.get_diam(), best_info.get_haspl());
            cout << buff << flush;

            best_graph = tmp;
            best_info = new_info;
            cnt = 0;

            // cout << endl;
            // cout << "[Log]" << endl;
            // cout << best_info.get_diam() << endl;
            // cout << best_info.get_haspl() << endl;
        } else {
            Graph::set_seed(mt());
            graph = makeMDTgraph(tmp);
            graph.linkLoops();
            // graph = Graph::make(s, h, r, 1);
            // graph = Graph::makeRandom(s, h, r);
            ++cnt;
        }
    }

    return best_graph;
}
#endif
