#ifndef INCLUDE_PATHTREE_HPP
#define INCLUDE_PATHTREE_HPP

#include<iostream>
#include<vector>

#include"Edge.hpp"


using namespace std;

class TreeNode {
    public:

    int no;
    int level;
    vector<Edge> uppers;
    vector<Edge> belows;

    TreeNode(int no, int level) : no(no), level(level) {}
    TreeNode() : TreeNode(-1, -1) {}
    
    void print() const;
};

void TreeNode::print() const{
    cout << "no = " << no << ", level = " << level << endl;
    cout << "[uppers]" << endl;
    for ( Edge e : uppers ) e.print();
    cout << "[belows]" << endl;
    for ( Edge e : belows ) e.print();
}

class PathTree {
    int pno;
    vector<vector<TreeNode>> tree;

    public:
    PathTree(int pno, const vector<TreeNode>& tnodes ) : pno(pno) {
        int max_level = -1;

        for ( const auto& a: tnodes ) max_level = max(max_level, a.level);
        tree = vector<vector<TreeNode>>(max_level+1);
        for ( const auto& node : tnodes ) tree[node.level].push_back(node);
    }

    int get_max_level() { return tree.size()-1; }

    void print() const ;
};

void PathTree::print() const {
    for ( int i = 0; i < tree.size(); ++i ) {
        cout << "[level: " << i << "]" << endl;
        for ( TreeNode tn : tree[i] ) tn.print();
        cout << endl;
    }
}
#endif