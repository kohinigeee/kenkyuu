#ifndef INCLUDE_GRAPHRESULTS_HPP
#define INCLUDE_GRAPHRESULTS_HPP

#include<iostream>
#include<vector>
#include<map>

#include"Edge.hpp"
#include"PathTree.hpp"

using path_t = vector<pair<Edge,Edge>>;

pair<Edge,Edge> makeEdgePair(const Edge e1, const Edge e2 ) {
    Edge edge1 = e1, edge2 = e2;
    if ( !(edge1 < edge2 ) ) swap(edge1, edge2);
    return make_pair(edge1, edge2);
}

class GraphResult{
    vector<vector<long long>> dmtx;
    map< pair<Edge,Edge>, long long > edge_scores;
    vector<PathTree> trees;

    public: 
    GraphResult( vector<vector<long long>>& dmtx, map< pair<Edge,Edge>, long long>& edge_scores, vector<PathTree>& trees) : dmtx(dmtx), edge_scores(edge_scores), trees(trees) {}
    
    const vector<vector<long long>>& getDmtx() { return dmtx; }
    const map< pair<Edge,Edge>, long long >& getEdgeScores() { return edge_scores; }
    vector<PathTree>& getTrees() { return trees; }
};

class GraphInfo {
    GraphResult result;
    long long sumd, diam; 
    double haspl;
    pair<int, int> diam_tantens;
    vector<pair<int,int>> diam_tanten_pairs;

    public:
    GraphInfo( const GraphResult& result, long long sumd, long long diam, double haspl, pair<int,int> diam_tantens, vector<pair<int, int>>& pairs):result(result), sumd(sumd), diam(diam), haspl(haspl), diam_tantens(diam_tantens), diam_tanten_pairs(pairs) {}

    GraphResult get_result() { return result; }
    long long get_sumd() { return sumd; }
    long long get_diam() { return diam; }
    double get_haspl() { return haspl; }
    const vector<pair<int,int>>& get_pairs() { return diam_tanten_pairs; } 
    pair<int,int> get_tantens() { return diam_tantens; }
};

class BfsResult {
    int st; //bfsの始点の絶対番号
    vector<long long> d; //距離配列
    vector<pair<Edge,Edge>> path;
    map<pair<Edge,Edge>, long long> edge_scores;
    vector<TreeNode> tnodes;

    public:
    BfsResult( int st, vector<long long>& d, vector<pair<Edge,Edge>>& path, const vector<TreeNode>& tnodes, const map<pair<Edge,Edge>, long long>& edge_scores ) : st(st), d(d), path(path), tnodes(tnodes), edge_scores(edge_scores) {}
    BfsResult() {}

    int get_st() { return st; }
    vector<long long> get_d() { return d; }
    vector<pair<Edge,Edge>> get_path() { return path; }
    const map<pair<Edge,Edge>, long long>& get_scores() { return edge_scores; }
    const vector<TreeNode> get_tnodes() { return tnodes; }
};

//info1がinfo2より良いかを判定
bool compInfo( GraphInfo& info1, GraphInfo& info2 ) {
    if ( info1.get_diam() == info2.get_diam() ) return info1.get_haspl() < info2.get_haspl();
    return info1.get_diam() < info2.get_diam();
}

#endif