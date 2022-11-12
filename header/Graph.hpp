#ifndef INCLUDE_GRAPH_HPP
#define INCLUDE_GRAPH_HPP

#include"GraphPart.hpp"
#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include<cmath>
#include<queue>
#include<random>
#include<map>
#include<algorithm>

#define GRAPH__SEDD 10

using graph_info_t = map<string, long long>;

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

    
    int s, h, r, g; // 合計のs, h
    int us;
    vector<GraphPart> parts;

    enum S_Type prevtype; //前回のsimple_swingのタイプ
    bool canback; //巻き戻しをできるかどうか
    pair<Edge, Edge> rireki; //前回のsimple_swingの戻り値


    Graph( int s, int h,int r, int g ) : s(s), h(h), r(r), g(g) {
        if ( s < 0 || h < 0 || r < 0 || g < 1 ) throw IregalValueException("[Graph::constructer] Ireagal value for Graph");
        if ( (s-1)*2+h > s*r ) throw IregalValueException("[Graph::constructer] Ireagal value for Graph ( Graph dones't exists )");
        if ( s%g != 0 || h%g != 0 ) throw IregalValueException("[Graph::constructer] Ireagal value of g");
        parts = vector<GraphPart>();
        us = s/g;
        prevtype = S_Type::UNDEF;
        canback = false;
    }

    void add( GraphPart& part ) { parts.push_back(part); }

    //指定したエッジの構成が正しいかを判定する
    bool isPropEdge( Edge::edgeType , G_no, Node_no, Edge_no );

    public:

    //正しいグラフ構成かを判定する
    bool isPropGraph();

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
        DEB(DEB_LOW) { cout << "[Log] Graph::getEdge()" << endl;}
        if ( g_no.getNo() < 0 || g_no.getNo() >= parts.size() ) throw IregalManuplateException("[Graph::getEdge()] Ireagal value for g_no");
        if ( type == Edge::HOST ) return parts[g_no.getNo()].get_host(node_no).getEdge();
        if ( type == Edge::SWITCH || type == Edge::LOOP ) return parts[g_no.getNo()].get_switch(node_no).getEdge(edge_no);
        throw IregalManuplateException("[Graph::getEdge()] Ireagal value for type");
    }

    //引数のエッジの終端のノードにおいて対応するエッジを抽出
    inline Edge getEdge(const Edge& edge ) {
        DEB(DEB_LOW) { cout << "[Log] Graph::getEdge(edge)" << endl;}
        if ( edge.getType() == Edge::NONE ) throw IregalManuplateException("[Graph::getEdge()] Ireagal manuplate to getEdge");
        if ( edge.getType() == Edge::LOOP ) return edge;
        if ( edge.getType() == Edge::HOST ) return getEdge( edge.getType(), edge.getG(), edge.getNode(), Edge_no(0));
        return getEdge( edge.getType(), edge.getG(), edge.getNode(), edge.getEdge() );
    }

    int deleteHost(const G_no&, const Node_no&);
    void make();

    //単一辺のswing関数(外部からは呼ばない)
    private:
    pair<Edge, Edge> simple_swing ( const Edge a1, const Edge b1, S_Type& type );

    public:
    //履歴に残すsimple_swing関数
    void simple_swing( const Edge a1, const Edge b1) {
      this->rireki = simple_swing(a1, b1, prevtype); 
      this->canback = true;
    }

    //一つ前の(swing実行前)のグラフに戻す
    bool back();

    //対象グラフに対するswing関数 
    void swing(const Edge& a1, const Edge& b1);


    //グラフの距離行列を求める
    vector<vector<long long>> calcBFS( const long long INF );

    //グラフのSSPL(Sum of Shortest Path Length )を求める
    graph_info_t sumD();

    void print(string, string);

    bool operator==(const Graph& g);
    bool operator!=(const Graph& g);

    Graph& operator=(const Graph& g);

    //同じ形かの判定
    bool isSame(const Graph& g);

    //グラフ作成関数
    static Graph make(int s, int h, int r, int g);

    //Dot言語への変換
    static void toDot(string fname, const Graph& graph, double _rd);
};

bool Graph::isPropEdge(Edge::edgeType type, G_no g_no, Node_no node_no, Edge_no edge_no ) {
    DEB(DEB_LOW) { cout << "[Log] Graph::isPropEdge()" << endl;}
    Edge a = getEdge(type, g_no, node_no, edge_no);
    Edge b = getEdge(a);

    if ( b.getType() != type ) return false;
    if ( b.getG() != g_no ) return false;
    if ( b.getNode() != node_no) return false;

    if ( b.getType() == Edge::edgeType::HOST ) return true;
    if ( b.getEdge() != edge_no ) return false;
    return true;
}

bool Graph::isPropGraph() {
    DEB(DEB_HIGH) { cout << "[Log] Graph::isPropGraph()" << endl; }
    set<Edge> st;

    for ( int i = 0; i < g; ++i ) {
        GraphPart gp = parts[i];
        G_no g_no(i);

        for ( int j = 0; j < gp.get_ssize(); ++j ) {
            Node_no node_no(j);
            //Switchの構成がおかしい場合はFalse
            if ( !(gp.get_switch(node_no).isPropSwitch()) ) {
                DEB(DEB_MIDLE) { cout << "[isPropGraph] inconsistent Switch" << endl;}
                return false;

            for ( int k = 0; k < gp.get_r(); ++k ) {
                Edge_no edge_no(k);
                Edge e = getEdge(Edge::edgeType::SWITCH, g_no, node_no, edge_no);

                //同一のEdgeが存在する場合はFalse
                if ( st.find(e) != st.end() ) {
                    DEB(DEB_MIDLE) { cout << "[isPropGraph] Same Edge exists" << endl; }
                    return false;
                }
                st.insert(e);

                //Edgeの構成が矛盾している場合はFalse
                if ( !(isPropEdge(Edge::edgeType::SWITCH, g_no, node_no, edge_no) ) ) {
                    DEB(DEB_MIDLE) { cout << "[isPropGraph] inconsistent Edge" << endl;}
                    return false;
                }
                }
            }
        }

        for ( int j = 0; j < gp.get_hsize(); ++j ) {
            Node_no node_no(j);
            Edge e = getEdge(Edge::edgeType::HOST, g_no, node_no, Edge_no(0));

            if ( st.find(e) != st.end() ) {
                DEB(DEB_MIDLE) { cout << "[isPropGraph] Same Edge exists" << endl; }
                return false;
            }
            st.insert(e);

            if ( !(isPropEdge(Edge::edgeType::HOST, g_no, node_no, Edge_no(0)) ) )  {
                DEB(DEB_MIDLE) { cout << "[isPropGraph] inconsistent Edge" << endl;}
                return false;
            }
        }
    }
    return true;
}


int Graph::deleteHost(const G_no& g_no, const Node_no& node_no) { //消すホストの種番号、リスト番号
    if ( g_no.getNo() < 0 || g_no.getNo() >= parts.size() ) return 0;
    if ( node_no.getNo() < 0 || node_no.getNo() >= parts[g].get_hosts().size() ) return 0;

    parts[g_no.getNo()].deleteHost(node_no);
    return 1;
}

Graph Graph::make(int s, int h, int r, int g) {
    Graph graph(s, h, r, g);

    DEB(DEB_MIDLE) { cout << "start make Graph" << endl;}
    int su = s/g, hu = h/g;
    GraphPart gp(su, hu, r, G_no(0));
    DEB(DEB_MIDLE) { cout << "make first GraphPart" << endl; }
    gp.init();
    DEB(DEB_MIDLE) { cout << "init finished" << endl; }
    graph.add(gp);
    for ( int i = 1; i < g; ++i ) {
        GraphPart addedgp = graph.parts.back().clone();
        graph.add(addedgp);
    }
    DEB(DEB_MIDLE) { cout << "make finished" << endl;}

    vector<vector<Edge>> loops;
    for ( int i = 0; i < s; ++i ) {
        vector<Edge> vec;
        G_no g_no(i/graph.us); Node_no node_no(i%graph.us);
        for ( int j = 0; j < graph.r; j++ ) {
            Edge_no edge_no(j);
            const Edge& e = graph.getSwitch(g_no, node_no).getEdge(j);
            if ( e.getType() == Edge::edgeType::LOOP ) vec.push_back(e);
        }
        if ( vec.size() > 0 ) loops.push_back(vec);
    }

    DEB(DEB_MIDLE) { cout << "serched loops" << endl;}
    mt19937 mt;
    mt.seed(GRAPH__SEDD);
    vector<int> loopsId(loops.size());
    for ( int i = 0; i < loopsId.size(); ++i ) loopsId[i] = i;

    while(loopsId.size() >= 2 ) {

        uniform_int_distribution<int> dist(0, loopsId.size()-1);
        uniform_int_distribution<int> dist2(0, loopsId.size()-2);
        int x1 = dist(mt);
        swap(loopsId[x1], loopsId[loopsId.size()-1]);
        int x2 = dist2(mt);
        swap(loopsId[x2], loopsId[loopsId.size()-2]);


        int idno1 = loopsId.size()-1, idno2 = loopsId.size()-2;
        int lno1 = loopsId.back(), lno2 = loopsId[idno2]; 

        graph.simple_swing(loops[lno1].back(), loops[lno2].back()); 

        loops[lno1].pop_back(); loops[lno2].pop_back();
        if ( loops[lno1].size() == 0 ) loopsId.pop_back();
        if ( loops[lno2].size() == 0 ) {
            swap(loopsId[idno2], loopsId.back()); 
            loopsId.pop_back();
        }
    }
    DEB(DEB_MIDLE) { cout << "finisched making" << endl;}
    return graph;
}

//単一swing処理
pair<Edge, Edge> Graph::simple_swing ( Edge a1, Edge b1, S_Type& type ) {
    DEB(DEB_HIGH) { cout << "[Log]Graph::simple_swing() " << endl; }
     Edge a2 = getEdge(a1);
     Edge b2 = getEdge(b1);
     type = S_Type::UNDEF;

    DEB(DEB_HIGH) { cout << "test" << endl;}
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

    if ( type == S_Type::E0L0H2 ) return make_pair(a1, a2);//ダミー
    if ( type == S_Type::E2L0H0 ) {
        DEB(DEB_HIGH) { cout << "[Log] simple_edge::E2L0H0" << endl; }
     //A1にB1への辺を設定
     getSwitch( a2.getG(), a2.getNode()).setEdge(a2.getEdge(), b2);
     //B1にA1への辺を設定
     getSwitch( b2.getG(), b2.getNode()).setEdge(b2.getEdge(), a2);

     //A2にB2への辺を設定
     getSwitch( a1.getG(), a1.getNode()).setEdge(a1.getEdge(), b1);
     //B2にA2への辺を設定
     getSwitch( b1.getG(), b1.getNode()).setEdge(b1.getEdge(), a1);
     
     return make_pair(b1, b2);
    } else if ( type == S_Type::E1L0H1 ) {
        DEB(DEB_HIGH) { cout << "[Log] simple_edge::E1L0H1" << endl; }

    //b2がホストになるように
     if ( a2.getType() == Edge::HOST ) swap<Edge>(a1, a2);
     if ( b2.getType() == Edge::HOST ) swap<Edge>(b1, b2);
     if ( a1.getType() == Edge::HOST ) {
        swap<Edge>(a1, b1);
        swap<Edge>(a2, b2);
        }

    deleteHost(b1.getG(), b1.getNode());
    //B1にA1への辺を設定
    getSwitch(b2.getG(), b2.getNode()).setEdge(b2.getEdge(), a2);
    //A1にB1への辺を設定
    getSwitch(a2.getG(), a2.getNode()).setEdge(a2.getEdge(), b2);

    getPart(a1.getG()).addHost(a1.getNode(), a1.getEdge());

    Edge p2 = getSwitch(a1.getG(), a1.getNode()).getEdge(a1.getEdge());
    return make_pair(b2, p2);

    } if ( type == S_Type::E1L1H0 ) {
        DEB(DEB_HIGH) { cout << "[Log] simple_swing::E1L1H0" << endl; }
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
        return make_pair(b1, a1);

    } else if ( type == S_Type::E0L2H0 ) {
        DEB(DEB_HIGH) { cout << "[Log] simple_swing::E0L2H0" << endl;}
        a1.setType(Edge::SWITCH);
        b1.setType(Edge::SWITCH);

        getSwitch(a1.getG(), a1.getNode()).setEdge(a1.getEdge(), b1);
        getSwitch(b1.getG(), b1.getNode()).setEdge(b1.getEdge(), a1);
        return make_pair(a2, b2); //特殊パターン

    } else if ( type == S_Type::E0L1H1 ) {
        DEB(DEB_HIGH) { cout << "[Log] simple_swing::E0L1H1" << endl;}
        //b2がホストになるようswap
        if ( a2.getType() == Edge::HOST ) swap<Edge>(a1, a2);
        if ( b2.getType() == Edge::HOST ) swap<Edge>(b1, b2);
        if ( a1.getType() == Edge::HOST ) { swap<Edge>(a1, b1); swap<Edge>(a2,b2); }

        deleteHost(b1.getG(), b1.getNode());
        a1.setType(Edge::SWITCH);
        getPart(a1.getG()).addHost(a1.getNode(), a1.getEdge());

        Edge p1 = getSwitch(a2.getG(), a2.getNode()).getEdge(a2.getEdge());
        Edge p2 = getSwitch(b2.getG(), b2.getNode()).getEdge(b2.getEdge());

        return make_pair(p1, p2);
    }

    return make_pair(Edge::makeNone(), Edge::makeNone());
    //ダミー(基本ここまでいかない)
}

//履歴によって一つ前の状態に戻す処理
bool Graph::back() {
    DEB (DEB_HIGH) { cout << "[Log] start back()" << endl;}
    if ( !canback ) return false;

    canback = false;
    if ( this->prevtype == S_Type::UNDEF ) return false;
    if ( this->prevtype == S_Type::E0L0H2 ) return true;
    Edge a = this->rireki.first;
    Edge b = this->rireki.second;
    if ( this->prevtype == S_Type::E0L2H0 ) {
        getSwitch(a.getG(), a.getNode()).setEdge(a.getEdge(), a);
        getSwitch(b.getG(), b.getNode()).setEdge(b.getEdge(), b);
        return true;
    } 
    
    S_Type tmp;
    a.print("back-a");
    b.print("back-b");
    simple_swing(a,b,tmp);
    return true;
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

vector<vector<long long>> Graph::calcBFS( const long long empv = -1) {
    vector<vector<long long>> d(g*s, vector<long long>(g*s, empv));

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

graph_info_t Graph::sumD() {
    vector<vector<long long>> d = calcBFS();
    long long ans = 0;
    long long diam = 0;
    pair<long long, long long> e;
    
    for ( int i = 0; i < s; ++i ) {
        G_no g_no(i/us); Node_no node_no(i%us);
        long long ih = getSwitch(g_no, node_no).get_hsize();

        if ( ih >= 2 ) { 
            ans += ih*(ih+1);
            if ( diam < 2 ) {
                diam = 2;
                e = make_pair(i, i);
            }
        }
        for ( int j = i+1; j < s; ++j ) {
            G_no to_gno(j/us); Node_no to_node(j%us);
            long long toh = getSwitch(to_gno, to_node).get_hsize();
            if ( toh == 0 ) continue;
            ans += ih*toh*(d[i][j]+2);
            if ( diam < d[i][j]+2 ) {
                diam = d[i][j]+2;
                e=make_pair(i, j);
            }
        }
    }

    map<string, long long> v = { {"sumd",ans}, {"diam", diam}, {"v1", e.first}, {"v2", e.second}};
    return v;
}


bool Graph::operator==(const Graph& graph ) {
    DEB(DEB_LOW) { cout << "[Log] Graph operator==" << endl;}
    if ( this->s != graph.s ) return false;
    if ( this->h != graph.h ) return false;
    if ( this->r != graph.r ) return false;
    if ( this->g != graph.g ) return false;
    if ( this->us != graph.us ) return false;

    for ( int i = 0; i < this->g; ++i ) {
        if ( this->parts[i] != graph.parts[i] ) return false;
    }
    return true;
}

bool Graph::operator!=(const Graph& g ) {
    return !((*this)==g);
}

Graph& Graph::operator=(const Graph& g ) {
    s = g.s; h = g.h; r = g.r; this->g = g.g;
    parts = g.parts;

    prevtype = g.prevtype;
    canback = g.canback;
    rireki = g.rireki;
    return (*this);
}

bool Graph::isSame(const Graph& graph) {
    if ( this->s != graph.s ) return false;
    if ( this->h != graph.h ) return false;
    if ( this->r != graph.r ) return false;
    if ( this->g != graph.g ) return false;
    if ( this->us != graph.us ) return false;

    for ( int i = 0; i < this->g; ++i ) {
        if ( !(parts[i].isSame(graph.parts[i])) ) return false;
    }
    return true;
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
    cout << flush;
}


//Dot言語に変換する関数
//[注意] print()関数の直後に実行すると出力がバグる
void Graph::toDot( string fname, const Graph& graph, double _rd = 3 ) {
    cout << flush;
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