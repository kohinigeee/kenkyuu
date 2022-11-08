#ifndef INCLUDE_GRAPH_HPP
#define INCLUDE_GRAPH_HPP

#include"GraphPart.hpp"
#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include<cmath>
#include<queue>

class Graph {
    enum S_Type {
        E2L0H0, //SWITCH間の辺二つ
        E1L0H1, //SWITCH間の辺一つ、ホストとの辺一つ
        E1L1H0, //SWITCH間の辺一つ、自己ループが一つ
        E0L2H0, //自己ループが二つ
        E0L1H1, //自己ループ一つ、ホストとの辺一つ
        E0L0H2, //ホストとの辺二つ(意味ない)
        UNDEF,
    };

    const int s, h, r, g; // 合計のs, h
    vector<GraphPart> parts;

    Graph( int s, int h,int r, int g ) : s(s), h(h), r(r), g(g) {
        if ( s < 0 || h < 0 || r < 0 || g < 1 ) throw IregalValueException("[Graph::constructer] Ireagal value for Graph");
        if ( (s-1)*2+h > s*r ) throw IregalValueException("[Graph::constructer] Ireagal value for Graph ( Graph dones't exists )");
        if ( s%g != 0 || h%g != 0 ) throw IregalValueException("[Graph::constructer] Ireagal value of g");
        parts = vector<GraphPart>();
    }

    void add( GraphPart& part ) { parts.push_back(part); }

    public:

    inline GraphPart& getPart( const G_no& g_no ) {
        if ( g_no.getNo() >= parts.size() ) throw IregalManuplateException();
        return parts[g_no.getNo()]; 
    }
    inline const GraphPart& getPart( const G_no& g_no ) const { 
        if ( g_no.getNo() >= parts.size() ) throw IregalManuplateException();
        return parts[g_no.getNo()];
    }
    inline Switch& getSwitch( const G_no& g_no, const Node_no& node_no ) { 
        return getPart(g_no).get_switch(node_no);
    }
    inline int getSum_s() const { return s; }
    inline int getSum_h() const{ return h; }
    inline int getSum_g() const { return g; }
    inline int get_r()  const { return r; }

    //指定のエッジを取り出す( type:ノードタイプ g_no:種番号 node_no:相対ノード番号 edge_no:エッジ番号)
    inline Edge getEdge(Edge::edgeType type, G_no g_no, Node_no node_no, Edge_no edge_no ) {
        DEB() { cout << "[Log] Graph::getEdge()" << endl;}
        if ( g_no.getNo() < 0 || g_no.getNo() >= parts.size() ) throw IregalManuplateException("[Graph::getEdge()] Ireagal value for g_no");
        if ( type == Edge::HOST ) return parts[g_no.getNo()].get_host(node_no).getEdge();
        if ( type == Edge::SWITCH || type == Edge::LOOP ) return parts[g_no.getNo()].get_switch(node_no).getEdge(edge_no);
        throw IregalManuplateException("[Graph::getEdge()] Ireagal value for type");
    }

    //引数のエッジの終端のノードにおいて対応するエッジを抽出
    inline Edge getEdge(const Edge& edge ) {
        DEB() { cout << "[Log] Graph::getEdge(edge)" << endl;}
        if ( edge.getType() == Edge::NONE ) throw IregalManuplateException("[Graph::getEdge()] Ireagal manuplate to getEdge");
        if ( edge.getType() == Edge::LOOP ) return edge;
        if ( edge.getType() == Edge::HOST ) return getEdge( edge.getType(), edge.getG(), edge.getNode(), Edge_no(0));
        return getEdge( edge.getType(), edge.getG(), edge.getNode(), edge.getEdge() );
    }

    int deleteHost(const G_no&, const Node_no&);
    void make();
    void simple_swing ( const Edge a1, const Edge b1 );
    void swing(const Edge& a1, const Edge& b1);
    vector<vector< long long>> calcBFS( const long long INF );
    void print(string, string);
    static Graph make(int s, int h, int r, int g);
    static void toDot(string fname, const Graph& graph, double _rd);
};


int Graph::deleteHost(const G_no& g_no, const Node_no& node_no) { //消すホストの種番号、リスト番号
    if ( g_no.getNo() < 0 || g_no.getNo() >= parts.size() ) return 0;
    if ( node_no.getNo() < 0 || node_no.getNo() >= parts[g].get_hosts().size() ) return 0;

    parts[g_no.getNo()].deleteHost(node_no);
    return 1;
}

Graph Graph::make(int s, int h, int r, int g) {
    Graph graph(s, h, r, g);

    DEB() { cout << "start make Graph" << endl;}
    int su = s/g, hu = h/g;
    GraphPart gp(su, hu, r, G_no(0));
    DEB() { cout << "make first GraphPart" << endl; }
    gp.init();
    DEB() { cout << "init finished" << endl; }
    graph.add(gp);
    for ( int i = 1; i < g; ++i ) {
        GraphPart addedgp = graph.parts.back().clone();
        graph.add(addedgp);
    }
    DEB() { cout << "make finished" << endl;}
    return graph;
}

//単一swing処理
void Graph::simple_swing ( Edge a1, Edge b1 ) {
    DEB() { cout << "[Log]Graph::simple_swing() " << endl; }
     Edge a2 = getEdge(a1);
     Edge b2 = getEdge(b1);
     enum S_Type type = S_Type::UNDEF;

    DEB() { cout << "test" << endl;}
    int es = 0, el = 0, eh = 0;
    if ( a1.getType() == Edge::LOOP ) ++el;
    if ( b1.getType() == Edge::LOOP ) ++el;
    if ( a1.getType() == Edge::HOST || a2.getType() == Edge::HOST ) ++eh;
    if ( b1.getType() == Edge::HOST || b2.getType() == Edge::HOST ) ++eh;
    es = 2-(el+eh);

    
    if ( es == 2 ) type = S_Type::E2L0H0;
    else if ( el == 2 ) type = S_Type::E0L2H0;
    else if ( eh == 2 ) type = S_Type::E0L0H2;
    else if ( es == 1 && el == 1 ) type = S_Type::E1L1H0;
    else if ( es == 1 && eh == 1 ) type = S_Type::E1L0H1;
    else if ( el == 1 && eh == 1 ) type = S_Type::E0L1H1;

    if ( type == S_Type::E0L0H2 ) return;
    if ( type == S_Type::E2L0H0 ) {
        cout << "[Log] simple_edge::E2L0H0" << endl;
     getSwitch( a2.getG(), a2.getNode()).setEdge(a2.getEdge(), b2);
     getSwitch( b2.getG(), b2.getNode()).setEdge(b2.getEdge(), a2);

     getSwitch( a1.getG(), a1.getNode()).setEdge(a1.getEdge(), b1);
     getSwitch( b1.getG(), b1.getNode()).setEdge(b1.getEdge(), a1);
    } else if ( type == S_Type::E1L0H1 ) {
        DEB() { cout << "[Log] simple_edge::E1L0H1"; }

     if ( a2.getType() == Edge::HOST ) swap<Edge>(a1, a2);
     if ( b2.getType() == Edge::HOST ) swap<Edge>(b1, b2);
     if ( a1.getType() == Edge::HOST ) {
        swap<Edge>(a1, b1);
        swap<Edge>(a2, b2);
        }

    deleteHost(b1.getG(), b1.getNode());
    getSwitch(b2.getG(), b2.getNode()).setEdge(b2.getEdge(), a2);
    getSwitch(a2.getG(), a2.getNode()).setEdge(a2.getEdge(), b2);

    getPart(a1.getG()).addHost(a1.getNode(), a1.getEdge());
    // parts[a1.to_g].addHost(getSwitch(a1.to_g, a1.to_no), a1.to_edge_no, Host(a1));
    } if ( type == S_Type::E1L1H0 ) {
        DEB() { cout << "[Log] simple_swing::E1L1H0" << endl; }
        if ( a1.getType() == Edge::LOOP ) {
            swap<Edge>(a1, b1);
            swap<Edge>(a2, b2);
        }
        //b1,b2が自己ループ
        b1.setType(Edge::SWITCH); //修正?
        getSwitch(b1.getG(), b1.getNode()).setEdge(b1.getEdge(), a2);
        getSwitch(a2.getG(), a2.getNode()).setEdge(a2.getEdge(), b1);

        a1.setType(Edge::LOOP);
        getSwitch(a1.getG(), a1.getNode()).setEdge(a1.getEdge(), a1);
    } else if ( type == S_Type::E0L2H0 ) {
        DEB() { cout << "[Log] simple_swing::E0L2H0";}
        a1.setType(Edge::SWITCH);
        b1.setType(Edge::SWITCH);

        getSwitch(a1.getG(), a1.getNode()).setEdge(a1.getEdge(), b1);
        getSwitch(b1.getG(), b1.getNode()).setEdge(b1.getEdge(), a1);
    } else if ( type == S_Type::E0L1H1 ) {
        DEB() { cout << "[Log] simple_swing::E0L1H1";}
        //b2がホストになるようswap
        if ( a2.getType() == Edge::HOST ) swap<Edge>(a1, a2);
        if ( b2.getType() == Edge::HOST ) swap<Edge>(b1, b2);
        if ( a1.getType() == Edge::HOST ) { swap<Edge>(a1, b1); swap<Edge>(a2,b2); }

        deleteHost(b1.getG(), b1.getNode());
        a1.setType(Edge::SWITCH);
        getPart(a1.getG()).addHost(a1.getNode(), a1.getEdge());
    }
}

//対称なグラフに対するswing処理
//対称を破壊するswing辺をはじく必要がる(未実装)
void Graph::swing(const Edge& a, const Edge& b) {
    Node_no a_node = a.getNode(), b_node = b.getNode();
    Edge_no a_edge = a.getEdge(), b_edge= b.getEdge();

    for ( int i = 0; i < g; ++i ) {
        G_no a_g((a.getG().getNo()+i)%g), b_g((b.getG().getNo()+i)%g);
        Edge swaped_a = getEdge(Edge::edgeType::SWITCH, a_g, a_node, a_edge);
        Edge swaped_b = getEdge(Edge::edgeType::SWITCH, b_g, b_node, b_edge);
        simple_swing(swaped_a, swaped_b);
    }
}

vector<vector<long long>> Graph::calcBFS( const long long empv) {
    vector<vector<long long>> d(g*s, vector<long long>(g*s, empv));
    int us = s/g;

    for ( int i = 0; i < s; ++i ) d[i][i] = 0;

    for ( int i = 0; i < s; ++i ) {
        int gno = i/us, node_no = i%us;
        queue<int> que;

        que.push(i);
        while(!que.empty()) {
            int topno = que.front(); que.pop();
            int gno = topno/us, node_no = topno%us;

            const vector<Edge>& edges = getSwitch(G_no(gno), Node_no(node_no)).getEdges();
            for ( const Edge& e : edges ) {
                if ( e.getType() != Edge::edgeType::SWITCH ) continue;
                int to_g = e.getG().getNo(), to_node = e.getNode().getNo();
                int to_sno = to_g*us+to_node;
                if ( d[i][to_sno] == empv ) {
                    d[i][to_sno] = d[i][topno]+1;
                    que.push(to_sno);
                }
            }
        }
    }

    return d;
}
void Graph::print(string name = "Graph", string stuff="" ) {
    string tmp = stuff+"   ";
    cout << stuff << '[' + name + ']' << endl;
    cout << tmp << "s_size=" << s << " h_size=" << h << " g=" << g << " r=" << r << endl;
    cout << endl;
    for ( int i = 0; i < parts.size(); ++i ) {
        parts[i].print("GraphPart "+to_string(i), tmp);
        cout << endl;
    }
}

void Graph::toDot( string fname, const Graph& graph, double _rd = 3 ) {
    ofstream ofs(fname);
    if ( !ofs ) {
        cout << fname << " が開けませんでした" << endl;
        return;
    }

    char str[256];

    ofs << "graph test2 {\n";
    ofs << "\tgraph [\n";
    ofs << "\t\tcharset=\"utf-8\",\n";
    ofs << "\t\tlayout=neato\n";
    ofs << "\t];\n";

    ofs << "\tnode [\n";
    ofs << "\t\tshape=box;\n";
    ofs << "\t\tstyle=\"solid, filled\"\n";
    ofs << "\t\tcolorscheme = paired12";
    ofs << "\t];\n\n";

    const int s_size = graph.getSum_s();//合計のスイッチ数
    const int h_size = graph.getSum_h();//合計のホスト数
    const int g = graph.g, s = s_size/g, h = h_size/g;
    const int colors = 12;
    const double theta_unit = (360.0)/s_size;
    double cx, cy; //中心座標
    double rd; //半径
    double theta = 90;

    cx = 5, 5;
    rd = _rd;

    //スイッチの配置を記述
    for ( int i = 0; i < g; ++i ) {
        const GraphPart& gp = graph.getPart(i);
        int color_no = i%colors+1;
        ofs << "\tsubgraph cluster_sub" << i << " {\n";
        ofs << "\tnode[ color=" << color_no << " ]\n";

        for ( int j = 0; j < s; ++j ) {
            const Switch& sw = gp.get_switch(j);
            double temp_theta = theta/180*M_PI;
            double tmp_cx = cx + rd*cos(temp_theta);
            double tmp_cy = cy + rd*sin(temp_theta);
            int s_abs = i*s+j;

            ofs << '\t' << s_abs << "[\n";
            sprintf_s(str, "\t\tpos=\"%0.3lf, %0.3lf!\", \n", tmp_cx, tmp_cy);
            ofs << str;
            sprintf_s(str, "\t\tlabel=\"%d (%d)", s_abs, sw.get_hsize());
            ofs << str;
            if ( sw.get_loop() > 0 ) ofs << " [" << sw.get_loop() << ']';
            ofs << "\"\n";
            ofs << "\t]\n";
            theta -= theta_unit;
            if ( theta < 0 ) theta += 360;
        }
        ofs << "\t}\n";
    }
    ofs << "\n";


    vector<vector<int>> visited = vector<vector<int>>(g, vector<int>(s, 0));

    //エッジを出力
    for ( int i = 0; i < g; ++i )  {
        const GraphPart& gp = graph.getPart(i);
        for ( int j = 0; j < s; ++j ) {
            const Switch sw = gp.get_switch(j);
            int sno_abs = i*s+j;
            for ( int k = 0; k < graph.r; ++k ) {
                const Edge& e = sw.getEdge(k);

                if ( e.getType() != Edge::SWITCH ) continue;
                int to_g = e.getG().getNo(), to_node = e.getNode().getNo();
                if ( visited[to_g][to_node] ) continue;
                int no_abs = to_g*s+to_node;
                sprintf_s(str, "\t%d--%d\n", sno_abs, no_abs);
                ofs << str;
            }
            visited[i][j] = 1;
        }
    }
    ofs << "}";

    ofs.close();
}

#endif