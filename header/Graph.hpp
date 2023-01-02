#ifndef INCLUDE_GRAPH_HPP
#define INCLUDE_GRAPH_HPP

#include"GraphPart.hpp"
#include"Exeception.hpp"
#include"GraphResult.hpp"
#include"PathTree.hpp"

#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include<cmath>
#include<queue>
#include<random>
#include<map>
#include<algorithm>

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

    static int graph_seed;
    
    int s, h, r, g; // 合計のs, h
    int us;
    vector<GraphPart> parts;

    enum S_Type prevtype; //前回のsimple_swingのタイプ
    bool canback; //巻き戻しをできるかどうか
    pair<Edge, Edge> rireki; //前回のsimple_swingの戻り値

    public:
    Graph( int s, int h,int r, int g ) : s(s), h(h), r(r), g(g) {
        if ( s < 0 || h < 0 || r < 0 || g < 1 ) throw IregalValueException("[Graph::constructer] Ireagal value for Graph");
        // if ( s%g != 0 || h%g != 0 ) throw IregalValueException("[Graph::constructer] Ireagal value of g");
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
    inline int get_us() const { return us; }

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

    //単一辺のswing関数(外部からは呼ばない)
    private:
    pair<Edge, Edge> simple_swing ( const Edge a1, const Edge b1, S_Type& type, bool);

    vector<vector<Edge>> collectLoops(bool);

    public:
    //履歴に残すsimple_swing関数
    void simple_swing( const Edge a1, const Edge b1, bool isLockedOK = false) {
      this->rireki = simple_swing(a1, b1, prevtype, isLockedOK); 
      this->canback = true;
    }

    //一つ前の(swing実行前)のグラフに戻す
    bool back();

    //対象グラフに対するswing関数 
    void swing(const Edge& a1, const Edge& b1);

    BfsResult bfs( int node_no, const long long empv );

    //任意の頂点ペアのパスを取り出す
    vector<pair<Edge,Edge>> takePath( int st, int end );

    //グラフの距離行列を求める
    GraphResult calcBFS( const long long );

    //グラフのGraphInfoを求める
    GraphInfo calcInfo();
    GraphInfo calcInfo(GraphResult& result);

    //グラフのSSPL(Sum of Shortest Path Length )を求める
    graph_info_t sumD();

    double calcHaspl(long long sumd) {
        return double(sumd)/(h*(h-1)/2);
    }

    void print(string, string);

    bool operator==(const Graph& g);
    bool operator!=(const Graph& g);

    Graph& operator=(const Graph& g);

    //同じ形かの判定
    bool isSame(const Graph& g);

    //連結かの判定
    bool isLinking(); 

    //グラフ上のloop辺をランダムにつなげる
    void linkLoops(bool);

    //グラフ上のホスト辺以外を自己ループにする
    void loopInit(bool isIncludeLocked);

    //すべてのGraphPartを統合したグラフに変形する
    Graph integrate();

    //グラフ作成関数
    static Graph make(int s, int h, int r, int g);
    static Graph make2(int s, int h, int r, int g, int h_level);
    //分割アニーリング用(下位グラフ)
    static Graph make3(int s, int h, int r, int g, int reserved_r);
    //分割アニーリング用(上位グラフ)
    static Graph makeFromParts(int r, vector<GraphPart>& gps );

    static Graph makeRandom(int s, int h, int r );

    //Dot言語への変換
    static void toDot(string fname, const Graph& graph, double _rd = 3.0,  string label = "");

    static void set_seed(int seed) { graph_seed = seed; }

    //Graph同士の比較(直径とhasplによる辞書順: a < b ) ※つまり、aのグラフの方が良いグラフ
    static bool compare(Graph& a, graph_info_t& a_info, Graph& b, graph_info_t& b_info) {
        if ( a.getSum_s() != b.getSum_s() || a.getSum_h() != b.getSum_h() || a.get_r() != b.get_r() ) {
            string msg = "[Error] compare diffrent type graphs";
            throw IregalManuplateException(msg);
        }

        if ( a_info["diam"] == b_info["diam"] ) {
            double ahaspl = a.calcHaspl(a_info["sumd"]);
            double bhaspl = b.calcHaspl(b_info["sumd"]);
            return ahaspl < bhaspl;
        }
        return a_info["diam"] < b_info["diam"];
    }
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
                    DEB(DEB_MIDLE) { 
                        cout << "[isPropGraph] inconsistent Edge" << endl;
                        cout << "Switch :: g=" << g_no.getNo() << " node=" << node_no.getNo() << " edge=" << edge_no.getNo() << endl;
                        e.print("Incosistent Edge");
                    }
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
                DEB(DEB_MIDLE) { {
                    cout << "[isPropGraph] inconsistent Edge" << endl;}
                    cout << "Host :: g=" << g_no.getNo() << " node=" << node_no.getNo() <<  endl;
                    e.print("Inconsistent Edge");
                }
                return false;
            }
        }
    }
    DEB(DEB_MIDLE) { cout << "[isPropGraph] result OK" << endl; }
    return true;
}


int Graph::deleteHost(const G_no& g_no, const Node_no& node_no) { //消すホストの種番号、リスト番号
    if ( g_no.getNo() < 0 || g_no.getNo() >= parts.size() ) return 0;
    if ( node_no.getNo() < 0 || node_no.getNo() >= parts[g_no.getNo()].get_hosts().size() ) return 0;

    parts[g_no.getNo()].deleteHost(node_no);
    return 1;
}

//Graph上のループ辺をあつめる
vector<vector<Edge>> Graph::collectLoops(bool isLockedOk = false ) {
    vector<vector<Edge>> loops;
    for ( int i = 0; i < s; ++i ) {
        vector<Edge> vec;
        G_no g_no(i/us); Node_no node_no(i%us);
        for ( int j = 0; j < r; j++ ) {
            Edge_no edge_no(j);
            const Edge& e = getSwitch(g_no, node_no).getEdge(j);
            if ( isLockedOk == false && e.getStatus() == Edge::edgeStatus::LOCKED ) continue;
            if ( e.getType() == Edge::edgeType::LOOP ) vec.push_back(e);
        }
        if ( vec.size() > 0 ) loops.push_back(vec);
    }
    return loops;
}

//グラフ上のループ辺をランダムにつなげる
void Graph::linkLoops(bool isLockeOk = false ) {

    vector<vector<Edge>> loops;
    loops = collectLoops(isLockeOk);

    DEB(DEB_MIDLE) { cout << "serched loops" << endl;}
    mt19937 mt;
    mt.seed(graph_seed);
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

        simple_swing(loops[lno1].back(), loops[lno2].back(), isLockeOk); 

        loops[lno1].pop_back(); loops[lno2].pop_back();
        if ( loops[lno1].size() == 0 ) loopsId.pop_back();
        if ( loops[lno2].size() == 0 ) {
            swap(loopsId[idno2], loopsId.back()); 
            loopsId.pop_back();
        }
    }
}

//分割アニーリング用
//reserved_r : 上位グラフ用に確保するポート数
Graph Graph::make3(int s, int h, int r, int g, int reserved_r) {
    Graph graph(s, h, r, g);

    int su = s/g, hu = h/g;
    GraphPart gp(su, hu, r, G_no(0));
    gp.init();
    graph.add(gp);
    for ( int i = 1; i < g; ++i ) {
        GraphPart addedgp = graph.parts.back().clone();
        graph.add(addedgp);
    }

    vector<vector<Edge>> loops = graph.collectLoops();

    int max_size = 0;
    int sum_loops = 0;
    for ( auto v : loops ) {
        max_size = max(max_size, int(v.size()));
        sum_loops += v.size();
    }

    vector<vector<Edge>> loops2(max_size);
    for ( auto v : loops ) {
        for ( int i = 0; i < v.size(); ++i ) {
            loops2[i].push_back(v[i]);
        }
    } 

    if ( reserved_r > sum_loops ) {
        string msg = "[Error] Graph::make3()::not enough loop edges for reserved_r";
        cout << msg << endl;
        throw IregalValueException(msg);
    }
    int cnt = 0;
    for ( auto edges: loops2 ) {
        for ( auto edge : edges ) {
            if ( cnt >= reserved_r ) break;
            edge.setStatus(Edge::edgeStatus::LOCKED);
            graph.getSwitch(edge.getG(), edge.getNode()).setEdge(edge.getEdge(), edge);
            ++cnt;
        }
        if ( cnt >= reserved_r ) break;
    }

    graph.linkLoops();
    if ( !graph.isLinking() ) {
        string msg = "[Error] Graph::make3()::Can't make linking graph\n";
        cout << msg << endl;
        throw IregalValueException(msg);
    }
    return graph;
}

Graph Graph::make2(int s, int h, int r, int g, int h_level ) {
    Graph graph(s, h, r, g);

    int su = s/g, hu = h/g;
    GraphPart gp(su, hu, r, G_no(0));
    gp.init2(h_level);
    graph.add(gp);
    for ( int i = 1; i < g; ++i ) {
        GraphPart addedgp = graph.parts.back().clone();
        graph.add(addedgp);
    }
    graph.linkLoops();
    if ( !graph.isLinking() ) {
        string msg = "[Error] Graph::make2()::Can't make linking graph\n";
        cout << msg << endl;
        throw IregalValueException(msg);
    }
    return graph;
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
    graph.linkLoops();

    DEB(DEB_MIDLE) { cout << "finisched making" << endl;}
    if ( !graph.isLinking() ) {
        string msg = "[Error] Graph::make()::Can't make linking graph\n";
        cout << msg << endl;
        graph.toDot("graph1.dot", graph, 3);
        throw IregalValueException(msg);
    }
    return graph;
}

Graph Graph::makeFromParts(int r, vector<GraphPart>& gps ) {
    int g = gps.size();
    int s = 0;
    int h = 0;
    for ( const auto& gp : gps ) { s += gp.get_ssize(); h += gp.get_hsize(); }

    Graph graph(s, h, r, g);

    for ( int i = 0; i < g; ++i ) {
        GraphPart tmp = gps[i].changeGno(G_no(i));
        graph.add(tmp);
    }
    return graph;
}

Graph Graph::makeRandom(int s, int h, int r) {
    mt19937 mt;
    mt.seed(graph_seed);
    vector<int> tree = makeRandomTree(s, r, mt);

    Graph graph(s, h, r, 1);
    GraphPart gp(s, h, r, 0);
    vector<int> cnt(s, 0);

    for ( int i = 0; i < s; ++i ) {
        if ( tree[i] == -1 ) continue;
        int p = tree[i];
        Edge e1 = Edge::makeToSwitich(G_no(0), Node_no(i), Edge_no(cnt[i]));
        Edge e2 = Edge::makeToSwitich(G_no(0), Node_no(p), Edge_no(cnt[p]));

        gp.get_switch(Node_no(p)).setEdge(Edge_no(cnt[p]), e1);
        gp.get_switch(Node_no(i)).setEdge(Edge_no(cnt[i]), e2);
        ++cnt[i]; ++cnt[p];
    }

    vector<int> pools;
    for ( int i = 0; i < s; ++i ) if ( cnt[i] < r ) pools.push_back(i);
    for ( int i = 0; i < h; ++i ) {
        if ( pools.size() == 0 ) {
            string msg = "[Error] Graph::makeRandom not enough ports to make graph";
            cout << msg << endl;
            throw IregalValueException(msg);
        }

        uniform_int_distribution<int> div(0, pools.size()-1);
        int idx = div(mt);
        int no = pools[idx];

        gp.get_switch(Node_no(no)).setEdge(Edge_no(cnt[no]), Edge::makeToHost(G_no(0), Node_no(i)));
        gp.get_host(Node_no(i)).setEdge(Edge::makeToSwitich(G_no(0), Node_no(no), Edge_no(cnt[no])));
        cnt[no] += 1;
        if ( cnt[no] >= r ) {
            swap(pools[idx], pools.back());
            pools.pop_back();
        }
    }

    for ( int i = 0; i < s; ++i ) {
        Switch& sw = gp.get_switch(Node_no(i));
        for ( int j = cnt[i]; j < r; ++j ) {
            sw.setEdge(Edge_no(j), Edge::makeToLoop(G_no(0), Node_no(i), Edge_no(j)));
        }
    }

    graph.add(gp);
    graph.linkLoops();

    if ( !graph.isLinking() ) {
        string msg = "[Error] Graph::makeRandom made not linking graph";
        cout << msg << endl;
        throw IregalManuplateException(msg);
    }

    return graph;
}

//単一swing処理
pair<Edge, Edge> Graph::simple_swing ( Edge a1, Edge b1, S_Type& type, bool isLockedOk = false ) {
    DEB(DEB_HIGH) { cout << "[Log]Graph::simple_swing() " << endl; }
     Edge a2 = getEdge(a1);
     Edge b2 = getEdge(b1);
     type = S_Type::UNDEF;
     
     if ( isLockedOk == false ) {
        if ( a1.getStatus() == Edge::edgeStatus::LOCKED || b1.getStatus() == Edge::edgeStatus::LOCKED  ) {
                cout << "[Eorror] simple_swing():: swing locked edge" << endl;
                throw IregalManuplateException("swing locked edge");
        }     
     }

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

//すべてのGraphPartのスイッチ数が均等なこと前提
BfsResult Graph::bfs( int node_no , const long long empv = -1 ) {
    vector<long long> d(s, empv);
    vector<pair<Edge,Edge>> path(s);
    // index i :  first i->pのエッジ p(parant)
    //         : second p->iのエッジ
    vector<TreeNode> tnodes;
    map<pair<Edge,Edge>, long long> edge_scores;

    queue<int> que;
    for ( int i = 0; i < getSum_s(); ++i ) {
        tnodes.push_back(TreeNode(i, -1));
    }

    d[node_no] = 0; 
    que.push(node_no);
    tnodes[node_no].level = 0;

    while(!que.empty()) {
        int topno = que.front(); que.pop();
        int gno = topno/us, nno = topno%us;

        const vector<Edge>& edges = getSwitch(G_no(gno), Node_no(nno)).getEdges();
        for ( const Edge& e : edges ) {
           if ( e.getType() != Edge::edgeType::SWITCH ) continue;

           int to_g = e.getG().getNo(), to_node = e.getNode().getNo();
           int to_sno = to_g*us+to_node;
           if ( d[to_sno] == empv ) {
            d[to_sno] = d[topno]+1;
            que.push(to_sno);

            path[to_sno] = make_pair(getEdge(e),e);
            
            edge_scores[makeEdgePair(e, getEdge(e))] += 1;
            //PathTree用処理
            tnodes[to_sno].level = d[to_sno];
           }

           if ( d[to_sno] == d[topno]+1 ) {
            tnodes[topno].belows.push_back(e);
            tnodes[to_sno].uppers.push_back(getEdge(e));
           }
           if ( d[to_sno] == d[topno] ) {
            tnodes[topno].flats.push_back(e);
           }
        }
    }
    return BfsResult(node_no, d, path, tnodes, edge_scores);
}

//ある頂点ペアの最短パスを抜き出す
//[注意]すべてのGraphPartのスイッチ数が均等なこと前提
vector<pair<Edge,Edge>> Graph::takePath( int st, int end ) {
    if ( st < 0 || st >= s ) {
        cout << "[Error] Graph::takePath st is invalid value" << endl;
        exit(1);
    }
    if ( end < 0 || end >= s ) {
        cout << "[Error] Graph::takePath st is invalied value" << endl;
        exit(1);
    }

    BfsResult restult = bfs(end);
    vector<pair<Edge,Edge>> p = restult.get_path();
    vector<pair<Edge,Edge>> ans;
    int node_no = st;

    while( node_no != end ) {
        ans.push_back(p[node_no]);

        Edge to_p = p[node_no].first; 
        int to_g = to_p.getG().getNo();
        int to_s = to_p.getNode().getNo();
        int to_node = to_g*us+to_s;

        node_no = to_node;
    }
    return ans;
}

GraphResult Graph::calcBFS( const long long empv = -1 ) {
    vector<vector<long long>> d;
    map<pair<Edge,Edge>, long long> edge_scores;
    vector<PathTree> trees;

    for ( int i = 0; i < s; ++i ) {
        BfsResult bresult = bfs(i, empv);
        d.push_back(bresult.get_d());

        for ( auto p : bresult.get_scores() ) {
            edge_scores[p.first] += p.second;
        } 

        trees.push_back(PathTree(i, bresult.get_tnodes()));
    }
    GraphResult ans = GraphResult(d, edge_scores, trees);
    return ans;
}

// GraphResult Graph::calcBFS( const long long empv = -1) {
//     //距離行列
    // vector<vector<long long>> d(g*s, vector<long long>(g*s, empv));
//     //各辺の重要度
//     map<pair<Edge,Edge>, long long> edge_scores;

//     for ( int i = 0; i < s; ++i ) d[i][i] = 0;
//     for ( int i = 0; i < s; ++i ) {
//         int gno = i/us, node_no = i%us;
//         queue<int> que;

//         que.push(i);
//         while(!que.empty()) {
//             //探索するスイッチの絶対番号
//             int topno = que.front(); que.pop();
//             int gno = topno/us, node_no = topno%us;

//             const vector<Edge>& edges = getSwitch(G_no(gno), Node_no(node_no)).getEdges();
//             for ( const Edge& e : edges ) {
//                 if ( e.getType() != Edge::edgeType::SWITCH ) continue;
//                 int to_g = e.getG().getNo(), to_node = e.getNode().getNo();
//                 int to_sno = to_g*us+to_node;

//                 if ( d[i][to_sno] == empv ) {
//                     d[i][to_sno] = d[i][topno]+1;
//                     que.push(to_sno);

//                     Edge edge2 = getEdge(e); 
//                     pair<Edge,Edge> pe = makeEdgePair(e, edge2);
//                     edge_scores[pe] += 1;
//                 }
//             }
//         }
//     }
//     GraphResult ans = GraphResult(d, edge_scores);
//     return ans;
// }

GraphInfo Graph::calcInfo() {
    GraphResult result = calcBFS();
    return calcInfo(result);
}

GraphInfo Graph::calcInfo(GraphResult& result) {
    vector<vector<long long>> d = result.getDmtx(); 
    long long ans = 0; //ホスト間の最短距離の総和
    long long sumd = 0; //スイッチ間の讃嘆距離の総和
    long long diam = 0;
    double haspl;
    pair<long long, long long> e;
    vector<pair<int,int>> diam_tanten_pairs;
    
    for ( int i = 0; i < s; ++i ) {
        G_no g_no(i/us); Node_no node_no(i%us);
        long long ih = getSwitch(g_no, node_no).get_hsize();

        if ( ih == 0 ) continue;
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
                diam_tanten_pairs.clear();
            }
            if ( diam == d[i][j]+2 ) {
                diam_tanten_pairs.push_back(make_pair(i,j));
            }
        }
    }

    haspl = ans/(double(h*(h-1)/2));
    return GraphInfo(result, ans,diam, haspl, e, diam_tanten_pairs);
}

graph_info_t Graph::sumD() {
    GraphResult result = calcBFS();
    vector<vector<long long>> d = result.getDmtx(); 
    long long ans = 0; //ホスト間の最短距離の総和
    long long sumd = 0; //スイッチ間の讃嘆距離の総和
    long long diam = 0;
    pair<long long, long long> e;
    
    for ( int i = 0; i < s; ++i ) {
        G_no g_no(i/us); Node_no node_no(i%us);
        long long ih = getSwitch(g_no, node_no).get_hsize();

        if ( ih == 0 ) continue;
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


//複数のGraphPartを1つに統合
Graph Graph::integrate() {
    vector<long long> sums(g,0);
    vector<long long> sumh(g,0);

    GraphPart ans(s, h, r, G_no(0));
    for ( int i = 1; i < g; ++i ) { 
        sums[i] = sums[i-1]+parts[i-1].get_ssize();
        sumh[i] = sumh[i-1]+parts[i-1].get_hsize();
    }

    for ( int i = 0; i < g; ++i ) {
        GraphPart& gp = getPart(G_no(i));
        for ( int j = 0; j < gp.get_ssize(); ++j ) {
            Switch sw = gp.get_switch(Node_no(j));
            int sno = sums[i]+j;

            for ( int k = 0; k < r; ++k ) {
                Edge& edge = sw.getEdge(Edge_no(k));
                int to_no;
                if ( edge.getType() == Edge::edgeType::HOST ) {
                    to_no = sumh[edge.getG().getNo()]+edge.getNode().getNo();
                } else to_no = sums[edge.getG().getNo()]+edge.getNode().getNo();

                edge.setG(G_no(0));
                edge.setNode(Node_no(to_no));
            }
            ans.get_switch(Node_no(sno)) = sw;
        }
        for ( int j = 0; j < gp.get_hsize(); ++j ) {
            int node_no = sumh[i]+j;
            Host h = gp.get_host(Node_no(j));
            Edge& edge = h.getEdge();
            int to_no = sums[edge.getG().getNo()]+edge.getNode().getNo();
            edge.setG(G_no(0));
            edge.setNode(Node_no(to_no));
            ans.get_host(Node_no(node_no)) = h;
        }
    }

    Graph graph = Graph(s, h, r, 1);
    graph.add(ans);
    return graph;
}

//ホスト辺以外の辺を全て自己ループにする
void Graph::loopInit( bool isIncludeLocked = true ) {
    for ( int i = 0; i < getSum_g(); ++i ) {
        GraphPart& gp = getPart(G_no(i));
        for ( int j = 0; j < gp.get_ssize(); ++j ) {
            Switch& sw = gp.get_switch(Node_no(j));
            for ( int k = 0; k < sw.get_r(); k++ ) {
                Edge e = sw.getEdge(Edge_no(k));
                if ( e.getType() == Edge::edgeType::HOST ) continue;
                if ( isIncludeLocked == false && e.getType() == Edge::edgeType::LOOP && e.getStatus() == Edge::edgeStatus::LOCKED ) {
                    continue;
                }
                e.setType(Edge::edgeType::LOOP);
                e.setG(gp.get_gno());
                e.setNode(Node_no(j));
                e.setEdge(Edge_no(k));
                sw.setEdge(Edge_no(k), e);
            }
        }
    }
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
    s = g.s; h = g.h; r = g.r; this->g = g.g; us = g.us;
    parts = g.parts;

    prevtype = g.prevtype;
    canback = g.canback;
    rireki = g.rireki;
    return (*this);
}

bool Graph::isSame(const Graph& graph) {
    DEB(DEB_HIGH) { cout << "[HIGH]Graph::isSame()" << endl;}
    if ( this->s != graph.s ) return false;
    if ( this->h != graph.h ) return false;
    if ( this->r != graph.r ) return false;
    if ( this->g != graph.g ) return false;
    if ( this->us != graph.us ) return false;

    for ( int i = 0; i < this->g; ++i ) {
        if ( !(parts[i].isSame(graph.parts[i])) ) {
            return false;
        }
    }
    return true;
}

bool Graph::isLinking() {
    vector<long long> d0(s);
    vector<long long> d1(s);

    d0 = bfs(0, -1).get_d();
    d1 = bfs(1, -1).get_d();

    for ( auto v : d0 ) if ( v == -1 ) return false;
    for ( auto v : d1 ) if ( v == -1 ) return false;
    return true;
}

void Graph::print(string name = "Graph", string stuff="" ) {
    string tmp = stuff+"   ";
    cout << stuff << '[' + name + ']' << endl;
    cout << tmp << "s_size=" << s << " h_size=" << h << " g=" << g << " r=" << r << " us = " << us << endl;
    cout << endl;
    for ( int i = 0; i < parts.size(); ++i ) {
        parts[i].print("GraphPart "+to_string(i), tmp);
        cout << endl;
    }
    cout << flush;
}


//Dot言語に変換する関数
//[注意] print()関数の直後に実行すると出力がバグる
void Graph::toDot( string fname, const Graph& graph, double _rd, string label) {
    cout << flush;
    ofstream ofs(fname);
    if ( !ofs ) {
        cout << fname << " が開けませんでした" << endl;
        return;
    }

    char str[256];
    if ( label == "" ) {
        label = "s:"+to_string(graph.getSum_s())+" h:"+to_string(graph.getSum_h())+" r:"+to_string(graph.get_r());
    }

    ofs << "graph test2 {\n";
    ofs << "\tgraph [\n";
    ofs << "\t\tcharset=\"utf-8\",\n";

    ofs << "\t\tlayout=neato,\n";
    ofs << "\t\tlabel=\"" << label << "\"";
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

    cx = 5, cy = 5;
    rd = _rd;

    //スイッチの配置を記述
    int node_fontsize = 14;
    for ( int i = 0; i < g; ++i ) {
        const GraphPart& gp = graph.getPart(i);
        int color_no = i%colors+1;
        ofs << "\tsubgraph cluster_sub" << i << " {\n";
        ofs << "\tnode[ color=" << color_no << " ]\n";
        ofs << "\tnode[\n";
        ofs << "\t\tcolor=" << color_no << "\n";
        ofs << "\t\tfontsize=" << node_fontsize<< "\n";
        ofs << "\t]\n";

        for ( int j = 0; j < s; ++j ) {
            const Switch& sw = gp.get_switch(j);
            double temp_theta = theta/180*M_PI;
            double tmp_cx = cx + rd*cos(temp_theta);
            double tmp_cy = cy + rd*sin(temp_theta);
            int s_abs = i*s+j;

            ofs << '\t' << s_abs << "[\n";
            sprintf_s(str, "\t\tpos=\"%0.5lf, %0.5lf!\", \n", tmp_cx, tmp_cy);
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

int Graph::graph_seed = 10;
#endif