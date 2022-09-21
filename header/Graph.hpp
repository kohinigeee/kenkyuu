#ifndef INCLUDE_GRAPH_HPP
#define INCLUDE_GRAPH_HPP

#include"GraphPart.hpp"
#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include<cmath>

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

    public:
    Graph( int _s, int _h,int _r, int _g ) : s(_s), h(_h), r(_r), g(_g) {
        parts = vector<GraphPart>();
        make();
    }
    
    void add( GraphPart& part ) { parts.push_back(part); }

    inline GraphPart& getPart( int no ) { return parts[no]; }
    inline const GraphPart& getPart( int no ) const { return parts[no]; }
    inline Switch& getSwitch( int g_no, int node_no ) { return getPart(g_no).get_switch(node_no); }
    inline int getSum_s() const { return s; }
    inline int getSum_h() const{ return h; }
    inline int getSum_g() const { return g; }
    inline int get_r()  const { return r; }

    //指定のエッジを取り出す( type:ノードタイプ g_no:種番号 node_no:相対ノード番号 edge_no:エッジ番号)
    inline Edge getEdge(int type, int g_no, int node_no, int edge_no = 0 ) {
        if ( type == HOST ) {
            return parts[g_no].get_host(node_no).getEdge();
        } else return parts[g_no].get_switch(node_no).getEdge(edge_no);
    }
    //引数のエッジの終端のノードにおいて対応するエッジを抽出
    inline Edge getEdge(const Edge& edge ) {
        int edge_no = 0;
        if ( edge.to_type != HOST ) edge_no = edge.to_edge_no;
        return getEdge( edge.to_type, edge.to_g, edge.to_no, edge.to_edge_no );
    }

    int deleteHost(int, int);
    void make();
    void swing ( const Edge a1, const Edge b1 );
    void print(string, string);
    static void toDot(string fname, const Graph& graph, double _rd);
};


int Graph::deleteHost(int g, int no) { //消すホストの種番号、リスト番号
    if ( g < 0 || g >= parts.size() ) return 0;
    if ( no < 0 || no >= parts[g].get_hosts().size() ) return 0;

    vector<Host>& hosts = parts[g].get_hosts();
    Edge tail = hosts.back().getEdge(); //最後尾のホストのエッジ
    Edge e = hosts[no].getEdge(); //削除するホストのエッジ
    parts[e.to_g].get_switch(e.to_no).setEdge(e.to_edge_no, Edge(LOOP, e.to_g, e.to_no, e.to_edge_no)); //削除されたホストを示すエッジをLOOPにする

    parts[tail.to_g].get_switch(tail.to_no).getEdge(tail.to_edge_no).to_no = no;
    hosts[no] = hosts.back();
    hosts.pop_back();
    return 1;
}

void Graph::make() {
        int su = s/g, hu = h/g;
    if ( g <= 0 ) { cout << "種数は1以上です" << endl; return; }
    GraphPart gp(su, hu, r, 0);
    gp.init();
        parts.push_back(gp);
    for ( int i = 1; i < g; ++i ) {
        parts.push_back(gp.clone());
    }
}

//単一swing処理
void Graph::swing ( Edge a1, Edge b1 ) {
    DEB() { cout << "swing() " << endl; }
     Edge a2 = getEdge(a1);
     Edge b2 = getEdge(b1);
     enum S_Type type = S_Type::UNDEF;

    int es = 0, el = 0, eh = 0;
    if ( a1.to_type == LOOP ) ++el;
    if ( b1.to_type == LOOP ) ++el;
    if ( a1.to_type == HOST || a2.to_type == HOST ) ++eh;
    if ( b1.to_type == HOST || b2.to_type == HOST ) ++eh;
    es = 2-(el+eh);

    
    if ( es == 2 ) type = S_Type::E2L0H0;
    else if ( el == 2 ) type = S_Type::E0L2H0;
    else if ( eh == 2 ) type = S_Type::E0L0H2;
    else if ( es == 1 && el == 1 ) type = S_Type::E1L1H0;
    else if ( es == 1 && eh == 1 ) type = S_Type::E1L0H1;
    else if ( el == 1 && eh == 1 ) type = S_Type::E0L1H1;

    cout << "a1: "; a1.print();
    cout << "a2: "; a2.print();
    cout << "b1: "; b1.print();
    cout << "b2: "; b2.print(); 
    cout << "swing: type = " << type << endl;

    if ( type == S_Type::E0L0H2 ) return;
    if ( type == S_Type::E2L0H0 ) {
     getSwitch( a2.to_g, a2.to_no).setEdge(a2.to_edge_no, b2);
     getSwitch( b2.to_g, b2.to_no).setEdge(b2.to_edge_no, a2);

     getSwitch( a1.to_g, a1.to_no).setEdge(a1.to_edge_no, b1);
     getSwitch( b1.to_g, b1.to_no).setEdge(b1.to_edge_no, a1);
    } else if ( type == S_Type::E1L0H1 ) {
        //ホストの操作に対応していない
        //b2がホストになるように置換
     if ( a2.to_type == HOST ) swap<Edge>(a1, a2);
     if ( b2.to_type == HOST ) swap<Edge>(b1, b2);
     if ( a1.to_type == HOST ) {
        swap<Edge>(a1, b1);
        swap<Edge>(a2, b2);
        }

    deleteHost(b1.to_g, b1.to_no);
    getSwitch(b2.to_g, b2.to_no).setEdge(b2.to_edge_no, a2);
    getSwitch(a2.to_g, a2.to_no).setEdge(a2.to_edge_no, b2);

    parts[a1.to_g].addHost(getSwitch(a1.to_g, a1.to_no), a1.to_edge_no, Host(a1));
    } if ( type == S_Type::E1L1H0 ) {
        if ( a1.to_type == LOOP ) {
            swap<Edge>(a1, b1);
            swap<Edge>(a2, b2);
        }
        //b1,b2が自己ループ
        b1.setType(SWITCH);
        getSwitch(b1.to_g, b1.to_no).setEdge(b1.to_edge_no, a2);
        getSwitch(a2.to_g, a2.to_no).setEdge(a2.to_edge_no, b1);

        a1.setType(LOOP);
        getSwitch(a1.to_g, a1.to_no).setEdge(a1.to_edge_no, a1);
    } else if ( type == S_Type::E0L2H0 ) {
        a1.setType(SWITCH);
        b1.setType(SWITCH);

        getSwitch(a1.to_g, a1.to_no).setEdge(a1.to_edge_no, b1);
        getSwitch(b1.to_g, b1.to_no).setEdge(b1.to_edge_no, a1);
    }
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

                if ( e.to_type != SWITCH ) continue;
                if ( visited[e.to_g][e.to_no] ) continue;
                int no_abs = e.to_g*s+e.to_no;
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