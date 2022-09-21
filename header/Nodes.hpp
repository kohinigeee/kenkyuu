#ifndef INCLUDE_NODES_HPP
#define INCLUDE_NODES_HPP

#include"Edge.hpp"
#include<vector>

using namespace std;

class Host {

    // int no_abs; //ホストの絶対番号
    Edge e; //スイッチへの辺

    public:
    Host(Edge _e ) : e(_e) {
    }

    Host() {}

    inline void setEdge(Edge _e ) { e = _e; }
    inline const Edge& getEdge() const { return e; }
    inline Edge& getEdge() { return e; }

    Host& operator= (const Host& h ){
        e = h.getEdge();
        return *this;
    }

    void print( string, string);
};

void Host::print( string name="Host", string stuff="")  {

    cout << stuff << '[' << name << "]" << "  "; e.print("Edge h");
}

bool operator< (const Host& h1, const Host& h2 ) {
    return (h1.getEdge() < h2.getEdge() );
}


class Switch {
    int r; //r:スイッチの接続上限、s:スイッチの絶対番号
    int h; //スイッチが接続しているホストの数
    int loop; //自己ループ(つまり使用していないポートの数)
    vector<Edge> edges; //スイッチが持つ辺を管理する配列

    public:
    Switch( int r1 ) : r(r1) {
        edges = vector<Edge>(r1);
        h = 0;
        loop = 0;
    }

    void setEdge( int no, Edge edge ) {
        if ( edges[no].to_type == HOST ) --h;
        if ( edges[no].to_type == LOOP ) --loop;
        edges[no] = edge;
        if ( edge.to_type == HOST ) ++h;
        if ( edge.to_type == LOOP ) ++loop;
    }

    inline int get_hsize() const { return h; }
    inline int get_r() const { return r; }
    inline int get_loop() const { return loop; }

    inline Edge& getEdge( int no ) { return edges[no]; }
    inline const Edge& getEdge( int no ) const{ return edges[no]; }
    inline const vector<Edge> getEdges() const { return edges; }

    Switch& operator=( const Switch& sw ) {
        r = sw.get_r(); h = sw.get_hsize(); loop = sw.get_loop();
        edges = sw.getEdges();
        return *this;
    }

    void print( string, string);
};

void Switch::print( string name="Switch", string stuff="") {

    string tmp = stuff+"  ";
    cout << stuff << '[' << name << "]\n";
    cout << tmp << "  " << "r=" << r << " hosts=" << h << endl;
    for ( int i = 0; i < edges.size(); ++i ) {
        cout << tmp << "  "; edges[i].print("Edge "+to_string(i));
    }
}
#endif