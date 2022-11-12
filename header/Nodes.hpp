#ifndef INCLUDE_NODES_HPP
#define INCLUDE_NODES_HPP

#include<vector>
#include<set>
#include<map>
#include<algorithm>
#include"Edge.hpp"
#include"functions.hpp"


using namespace std;

class Host {

    // int no_abs; //ホストの絶対番号
    Edge e; //スイッチへの辺

    public:
    Host(const Edge& e = Edge::makeNone()) : e(e) {}

    inline const Edge& getEdge() const { return e; }
    inline Edge& getEdge() { return e; }

    void setEdge (const Edge& e1 ) { 
        if ( e1.getType() != Edge::SWITCH ) throw IregalValueException("[Host::setEdge()] Ireagal value of e.type");
        this->e = e1;
    }

    Host& operator= (const Host& h ) {
        e = h.getEdge();
        return *this;
    }

    bool operator== ( const Host& h ) {
        return this->e == h.e;
    }

    bool operator!= ( const Host& h ) { return !((*this) == h ); }

    void print( string, string);
};

void Host::print( string name="Host", string stuff="")  {

    cout << stuff << '[' << name << "]" << "  "; e.print("Edge h");
}

class Switch {
    int r; //r:スイッチの接続上限
    int h; //スイッチが接続しているホストの数
    int loop; //自己ループの数(つまり使用していないポートの数)
    vector<Edge> edges; //スイッチが持つ辺を管理する配列

    public:
    Switch( int r ) : r(r) {
        if ( r < 0 ) throw IregalValueException("[Switch::constructer] r(port no) is not minus");
        edges = vector<Edge>(r, Edge::makeNone());
        h = 0;
        loop = 0;
    }

    void setEdge( const Edge_no& e_no, const Edge& edge ) {
        int no = e_no.getNo();

        if ( no >= r ) throw IregalManuplateException("[Switch::setEdge()] iregal no of Edges");
        if ( edges[no].getType() == Edge::HOST ) --h;
        if ( edges[no].getType() == Edge::LOOP ) --loop;
        edges[no] = edge;
        if ( edge.getType() == Edge::HOST ) ++h;
        if ( edge.getType() == Edge::LOOP ) ++loop;
    }

    inline int get_hsize() const { return h; }
    inline int get_r() const { return r; }
    inline int get_loop() const { return loop; }

    inline Edge& getEdge( Edge_no no ) { return edges[no.getNo()]; }
    inline const Edge& getEdge( Edge_no no ) const{ return edges[no.getNo()]; }
    inline const vector<Edge>& getEdges() const { return edges; }

    Switch& operator=( const Switch& sw ) {
        r = sw.get_r(); h = sw.get_hsize(); loop = sw.get_loop();
        edges = sw.getEdges();
        return *this;
    }

    bool operator==( const Switch& sw ) {
        DEB(DEB_LOW) { cout << "[Log] Switch operator==" << endl; }
        if ( this->r != sw.r ) return false;
        if ( this->h != sw.h ) return false;
        if ( this->loop != sw.loop ) return false;
        for ( int i = 0; i < r; ++i ) {
            if ( this->edges[i] != sw.edges[i] ) return false;
        }
        return true;
    }

    bool operator!=( const Switch& sw ) { return !( (*this) == sw ); }

    bool isSame(const Switch& sw);
    bool isPropSwitch();
    void print( string, string);
};

//正誤条件には注意
bool Switch::isSame(const Switch& sw) {
    if ( this->r != sw.r ) return false;
    if ( this->h != sw.h ) return false;
    if ( this->loop != sw.loop ) return false;

    vector<Edge> vec1, vec2;
    for ( int i = 0; i < r; ++i ) {
        Edge e = edges[i];
        if ( e.getType() == Edge::edgeType::HOST ) continue;
        e.setEdge(Edge_no(0));
        vec1.push_back(e);
    }
    for ( int i = 0; i < r; ++i ) {
        Edge e = sw.edges[i];
        if ( e.getType() == Edge::edgeType::HOST ) continue;
        e.setEdge(Edge_no(0));
        vec2.push_back(e);
    }
    
    sort(vec1.begin(), vec1.end());
    sort(vec2.begin(), vec2.end());
    return vec1 == vec2;
}

bool Switch::isPropSwitch() {
    if ( edges.size() != r ) return false;

    map<Edge::edgeType, int> mp;
    mp[Edge::edgeType::HOST] = 0;
    mp[Edge::edgeType::SWITCH] = 0;
    mp[Edge::edgeType::LOOP] = 0;

    for ( int i = 0; i < r; ++i ) mp[edges[i].getType()] += 1;
    if ( mp[Edge::edgeType::HOST] != h ) return false;
    if ( mp[Edge::edgeType::LOOP] != loop ) return false;
    return true;
}

void Switch::print( string name="Switch", string stuff="") {

    string tmp = stuff+"  ";
    cout << stuff << '[' << name << "]\n";
    cout << tmp << "  " << "r=" << r << " hosts=" << h << " loop=" << loop << endl;
    for ( int i = 0; i < edges.size(); ++i ) {
        cout << tmp << "  "; edges[i].print("Edge "+to_string(i));
    }
}
#endif