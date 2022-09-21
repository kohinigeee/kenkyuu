#ifndef INCLUDE_EDGE_HPP
#define INCLUDE_EDGE_HPP

#include<iostream>
#include<string>
#include"Debug.hpp"

#define NONE -1
#define HOST 1
#define SWITCH 2
#define LOOP 3

using namespace std;

class Edge {

    public:
    int to_type, to_g, to_no, to_edge_no;
    // to_type : 終点のノードのタイプ
    // to_g : 終点のノードが属する種の番号
    // to_no : 終点のノードの種内でのノード番号
    //    (HOST の場合はGraphPart(hosts)内でのインデックスをあらわす)
    // to_edge_no : 終点のノード内のエッジ番号 (ホストに対する場合は-1)

    Edge(int type, int g, int no, int edge_no ) : to_type(type), to_g(g), to_no(no), to_edge_no(edge_no) {
        
    }
    Edge() { Edge(-1, -1, -1, -1); }

    void inline setType(int type ) {
        this->to_type = type;
    }

    Edge& operator= (const Edge& e ) {
        to_type = e.to_type;
        to_g = e.to_g;
        to_no = e.to_no;
        to_edge_no = e.to_edge_no;
        return *this;
    }

    void print(string, string);
};

void Edge::print(string name="Edge", string stuff = "") {
    string type;
    string tmp = stuff;

    switch (to_type ) {
     case(HOST):
        type = "HOST  ";
        break;
     case(SWITCH):
        type = "SWICTH";
        break;
     case(NONE):
        type = "NONE  ";
        break;
     case(LOOP):
        type = "LOOP  ";
        break;
     default:
        type = "UNDEFINED";
    }

    tmp += '[' + name + "]{";
    tmp += " [to_type]: "+type;
    tmp += "  [to_g]: "+to_string(to_g);
    tmp += "  [to_no]: "+to_string(to_no);
    tmp += "  [to_edge_no]: "+to_string(to_edge_no);
    tmp += " }";
    cout << tmp << endl;
}

bool operator<(const Edge& e, const Edge& e2 ) {
    if ( e.to_type != e2.to_type ) return e.to_type < e2.to_type;
    if ( e.to_g != e2.to_g) return e.to_g < e2.to_g;
    if ( e.to_no != e2.to_no ) return e.to_no < e2.to_no;
    return e.to_edge_no < e2.to_edge_no;
}

#endif