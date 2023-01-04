#ifndef INCLUDE_DIRECTHILLCLIMB_HPP
#define INCLUDE_DIRECTHILLCLIMB_HPP

//指向性をもったエッジ選択
#include<set>
#include<algorithm>

#include "Graph.hpp"
#include "MDST.hpp"
#include "HillClimb.hpp"

using namespace std;
using directHillclimbMethod_t = bool(*) (Graph& graph);

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
bool directHillclimb_once_2(Graph& graph) {

    GraphResult result = graph.calcBFS();
    GraphInfo info = graph.calcInfo(result);
    bool isUpdated = false;

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
bool directHillclimb_once(Graph& graph) {

    GraphResult result = graph.calcBFS();
    GraphInfo info = graph.calcInfo(result);
    bool isUpdated = false;

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
Graph directHillclimb( Graph& graph_given, mt19937& mt, double alpha, directHillclimbMethod_t dmethod) {
    Graph graph = graph_given;
    set<pair<Edge,Edge>> iscalced;
    int cnt = 0;

    while(1) {
        if ( dmethod(graph) ) continue;;

        long long limt = alpha*countEdges(graph); 
        GraphInfo info = graph.calcInfo();
        long long cnt = 0;
        while( cnt < limt ) {
            // cout << "cnt = " << cnt << endl;
            pair<Edge,Edge> pe = select_edges_noraml(graph, mt); 

            if ( iscalced.find(pe) != iscalced.end() ) continue;
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
Graph directHillclimbWithKick( Graph& graph_given, mt19937& mt, int limcnt, directHillclimbMethod_t dmethod ) {
    Graph best_graph = graph_given;
    GraphInfo best_info = best_graph.calcInfo();
    int cnt = 0;
    double alpha = 0.8;
    Graph graph = best_graph;

    const int s = graph_given.getSum_s();
    const int h = graph_given.getSum_h();
    const int r = graph_given.get_r();

    while( cnt < limcnt ) {
        Graph tmp = directHillclimb(graph, mt, alpha, dmethod);
        GraphInfo new_info = tmp.calcInfo();
        
        cout << "[Log] cnt = "<< cnt << ", new : = " << new_info.get_diam() << ", " << new_info.get_haspl() << endl;
        if ( compInfo(new_info, best_info) ) {
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
