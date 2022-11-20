#ifndef INCLUDE_EDGE_HPP
#define INCLUDE_EDGE_HPP

#include<iostream>
#include<string>
#include"Debug.hpp"
#include"Exeception.hpp"

using namespace std;

class G_no {
    int g_no;

    public:
    G_no( const int no ) : g_no(no) {
        if ( no < 0 ) throw IregalValueException("[G_no::constructer] g_no is not minus value");
    }
    G_no() { G_no(0); }

    int inline getNo() const { return g_no; }

    G_no& operator= (const G_no& g ) {
        this->g_no = g.getNo();
        return (*this);
    }

    bool operator== ( const G_no& g ) {
        return this->g_no == g.getNo();
    }

    bool operator!= ( const G_no& g ) {
        return !(*this == g);
    }

    bool operator< ( const G_no& g ) {
        return g_no < g.g_no;
    }
};

class Node_no {
    int node_no;

    public:
    Node_no( const int no ) : node_no(no) {
        if ( no < 0 ) throw IregalValueException("[Node_no::constructer] node_no is not minus value");
    }
    Node_no() { Node_no(0); }

    int inline getNo() const { return node_no; }

    Node_no& operator= (const Node_no& node ) {
        this->node_no = node.getNo();
        return (*this);
    }

    bool operator== (const Node_no& node ) {
        return this->node_no == node.getNo();
    }

    bool operator!= (const Node_no& node ) {
        return !(*this == node);
    }

    bool operator< ( const Node_no& node ) {
        return node_no < node.node_no;
    }
};

class Edge_no {
    int edge_no;

    public:
    Edge_no ( const int no ) : edge_no(no) {
        if ( no < 0 ) throw IregalValueException("[Edge_no::constructer] edge_no is not minus value");
    }
    Edge_no () { Edge_no(0); }
    int inline getNo() const { return edge_no; }

    Edge_no& operator= (const Edge_no& edge) {
        this->edge_no = edge.getNo();
        return(*this);
    }

    bool operator== (const Edge_no& edge ) {
        return this->edge_no == edge.getNo();
    }

    bool operator!= (const Edge_no& edge ) {
        return !(*this == edge);
    }

    bool operator< (const Edge_no& edge ) {
        return edge_no < edge.getNo();
    }
};

class Edge {
    static const Edge_no hostDefaultEdge;
    static const Edge_no noneDefaultEdge;
    static const Node_no noneDefaultNode;
    static const G_no noneDefaultG;

    public:
     enum edgeType {
        NONE, HOST, SWITCH, LOOP
    };

    enum edgeStatus {
        LOCKED, UNLOCK
    };

    private:
    edgeType to_type;
    G_no to_g;
    Node_no to_node;
    Edge_no to_edge;
    edgeStatus status;
    // to_type : 終点のノードのタイプ
    // to_g : 終点のノードが属する種の番号
    // to_no : 終点のノードの種内でのノード番号
    //    (HOST の場合はGraphPart(hosts)内でのインデックスをあらわす)
    // to_edge_no : 終点のノード内のエッジ番号 (ホストに対する場合は-1)


    Edge( edgeType type, G_no g, Node_no node, Edge_no edge, edgeStatus sta= UNLOCK ) : to_type(type), to_g(g), to_node(node), to_edge(edge), status(sta){}

    public:
    Edge() { (*this) = Edge::makeNone(); }

    Edge static inline makeToSwitich( const G_no g_no, const Node_no node_no,const Edge_no edge_no, edgeStatus sta = edgeStatus::UNLOCK ) {
        return Edge(SWITCH, g_no, node_no, edge_no, sta);
    }

    Edge static inline makeToHost( const G_no g_no, const Node_no node_no, edgeStatus sta = edgeStatus::UNLOCK ) {
        return Edge(HOST, g_no, node_no, hostDefaultEdge, sta);
    }

    Edge static inline makeToLoop( const G_no g_no, const Node_no node_no, const Edge_no edge_no ) {
        return Edge(LOOP, g_no, node_no, edge_no);
    }

    Edge static inline makeNone() {
        return Edge(NONE, noneDefaultG, noneDefaultNode, noneDefaultEdge);
    }

    //接続先のノードタイプを取得
    edgeType inline getType() const;
    //接続先の種番号を取得
    G_no inline getG() const;
    //接続先のノード番号を取得
    Node_no inline getNode() const;
    //接続先のエッジ番号を取得
    Edge_no inline getEdge() const;

    edgeStatus inline getStatus() const;

    void inline setG(const G_no& g ) { this->to_g = g; }
    void inline setType( Edge::edgeType type ) { this->to_type = type; }
    void inline setEdge(const Edge_no& e_no) { this->to_edge = e_no;}
    void inline setStatus(const Edge::edgeStatus sta) { this->status = sta; }

    Edge& operator=(const Edge& e) {
        this->to_type = e.to_type;
        this->to_g = e.to_g;
        this->to_node = e.to_node;
        this->to_edge = e.to_edge;
        this->status = e.status;
        return *this;
    }


    void print(string, string) const;
};

const Edge_no Edge::hostDefaultEdge = Edge_no(0);
const Edge_no Edge::noneDefaultEdge = Edge_no(0);
const Node_no Edge::noneDefaultNode = Node_no(0);
const G_no Edge::noneDefaultG = G_no(0);

Edge::edgeType inline Edge::getType() const { return to_type; }
Edge::edgeStatus inline Edge::getStatus() const { return status; }

G_no inline Edge::getG() const { 
    if ( to_type == NONE )  throw IregalManuplateException("[Edge::getG()] his Edge have no 'to_g");
    return to_g; }

Node_no inline Edge::getNode() const { 
    if ( to_type == NONE )  throw IregalManuplateException("[Edge::getNode()] This Edge have no 'to_node");
    return to_node; }

Edge_no inline Edge::getEdge() const { 
    if ( to_type == NONE || to_type == HOST ) {
        cout << "[Error] Edge::getEdge()" << endl;
        throw IregalManuplateException("[Edge::getEdge()] This Edge have no 'to_edge"); 
    }
        
    return to_edge;
    }

void Edge::print(string name="Edge", string stuff = "") const{
    string type;
    string tmp = stuff;
    string s_no = (to_type != NONE ) ? to_string(to_node.getNo()) : "-";
    string s_g =  (to_type != NONE ) ? to_string(to_g.getNo()) : "-";
    string s_edge_no = ( to_type != NONE && to_type != HOST ) ? to_string(to_edge.getNo()) : "-";
    string s_status;

    vector<string> v_status = {"LOCKED", "UNLOCK"};
    vector<string> v_type = {"NONE", "HOST", "SWITCH", "LOOP"};
    s_status = v_status[status];
    type = v_type[to_type];

    tmp += '[' + name + "]{";
    tmp += " [to_type]: "+type;
    tmp += "  [to_g]: "+ s_g;
    tmp += "  [to_node]: "+ s_no;
    tmp += "  [to_edge_no]: "+ s_edge_no;
    tmp += "  [status] " + s_status;
    tmp += " }";
    cout << tmp << endl;
}

bool operator==(const Edge&e1, const Edge&e2) {
   if ( e1.getType() != e2.getType() ) return false;
   if ( e1.getType() == Edge::edgeType::NONE ) return true;
   if ( e1.getG() != e2.getG() ) return false;
   if ( e1.getNode() != e2.getNode() ) return false;
   if ( e1.getType() == Edge::edgeType::HOST ) return true;
   if ( e1.getEdge() != e2.getEdge() ) return false;
   if ( e1.getStatus() != e2.getStatus() ) return false;

   return true;
}

bool operator!=(const Edge&e1, const Edge&e2 ) {
    return !(e1==e2);
}

bool operator<(const Edge& e1, const Edge& e2) {
    if ( e1.getType() != e2.getType() ) return e1.getType() < e2.getType();
    if ( e1.getType() == Edge::edgeType::NONE ) return false;
    if ( e1.getG() != e2.getG() ) return e1.getG() < e2.getG();
    if ( e1.getNode() != e2.getNode() ) return e1.getNode() < e2.getNode();
    if ( e1.getType() == Edge::edgeType::HOST ) return false;
    if ( e1.getEdge() != e2.getEdge() ) return e1.getEdge() < e2.getEdge();
    return false;
}
#endif