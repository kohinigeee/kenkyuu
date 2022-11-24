#ifndef INCLUDE_GRAPHPART_HPP
#define INCLUDE_GRAPHPART_HPP

#include "Nodes.hpp"
#include <iostream>
#include <vector>
#include <string>
#include<algorithm>


//一つの種を表すクラス
class GraphPart {
    int s, h; //s : スイッチ数, h:ホスト数, g:種の番号
    int r;
    G_no g;
    vector<Switch> switchs;
    vector<Host> hosts;

    public:
    GraphPart(const int s, const int h, const int r, const G_no& g) : s(s), h(h), r(r), g(g) {
        DEB(DEB_MIDLE) { 
            cout << "constructer GraphPart" << endl;
            cout << "s = " << s << ", h = " << h << ", r = " << r << endl; }
        if ( s < 0 ) throw IregalValueException("[GraphPart::constructer] Ireagal value of s");
        if ( r < 0 ) throw IregalValueException("[GraphPart::constructer] Ireagal value of r");
        if ( h < 0 ) throw IregalValueException("[GraphPart::constructer] Ireagal value of h");
        if ( s*r < h+s ) throw IregalValueException("[GraphPart::constructer] Ireagal values for Graph");

        switchs = vector<Switch>();
        hosts = vector<Host>();
        for ( int i = 0; i < s; ++i ) {
            switchs.push_back(Switch(r));
        }
        for ( int i = 0; i < h; ++i ) {
            hosts.push_back(Host());
        }
    }

    inline int get_hsize() const { return h; }
    inline int get_ssize() const { return s; }
    inline int get_r() const { return r; }
    inline G_no get_gno() const { return g; }

    inline void set_gno( const G_no& g_no ) { 
        if ( g_no.getNo() < 0 ) throw IregalValueException("[GraphPart::set_gno()] Iregal value of g_no");
        g = g_no;
    }

    inline Host& get_host( const Node_no& no) { 
        if ( no.getNo() < 0 || no.getNo() >= hosts.size() ) throw IregalManuplateException();
        return hosts[no.getNo()];
    }
    inline const Host& get_host( const Node_no& no ) const { 
        if ( no.getNo() < 0 || no.getNo() >= hosts.size() ) throw IregalManuplateException();
        return hosts[no.getNo()];
    }

    inline Switch& get_switch(const Node_no& no) {
        if ( no.getNo() < 0 || no.getNo() >= switchs.size() ) throw IregalManuplateException();
        return switchs[no.getNo()];
    }
    inline const Switch& get_switch(const Node_no& no) const {
        if ( no.getNo() < 0 || no.getNo() >= switchs.size() ) throw IregalManuplateException();
        return switchs[no.getNo()];
    }

    inline const vector<Switch>& get_switchs() const { return switchs; }
    inline const vector<Host>& get_hosts() const { return hosts; }

    GraphPart clone(G_no g_no) {
        GraphPart gp = (*this);
        for ( int i = 0; i < s; ++i ) {
            Switch& sw = gp.get_switch(i);
            for ( int j = 0; j < sw.get_r(); ++j ) {
                Edge& e = sw.getEdge(Edge_no(j));
                e.setG(g_no);
            }
        }

        for ( int i = 0; i < h; ++i ) {
            Edge& e = gp.get_host(i).getEdge();
            e.setG(g_no);
        }
        gp.set_gno(g_no);
        return gp;
    }

    GraphPart clone() { return (*this).clone(G_no(g.getNo()+1)); }

    bool isSame(const GraphPart& gp); 

    GraphPart& operator= (const GraphPart& gp ) {
        s = gp.get_ssize(); h = gp.get_hsize(); g = gp.get_gno();
        r = gp.get_r();
        switchs = gp.get_switchs();
        hosts = gp.get_hosts();
        return *this;
    }

    bool operator== (const GraphPart& gp ) {
        DEB(DEB_LOW) { cout << "[Log] GraphPart operat==" << endl;}
        if ( this->s != gp.s ) return false;
        if ( this->h != gp.h ) return false;
        if ( this->r != gp.r ) return false;
        if ( this->g != gp.g ) return false;
        for ( int i = 0; i < s; ++i ) {
            if ( switchs[i] != gp.switchs[i] ) return false;
        }
        for ( int i = 0; i < h; ++i ) {
            if ( hosts[i] != gp.hosts[i] ) return false;
        }
        return true;
    }

    bool operator!=  (const GraphPart& gp ) { return !((*this) == gp);}

    void init2( int h_level, int r_leve);
    //種数1のグラフの生成
    void init();
    bool isRightEdge(const Edge&);
    Host& addHost(const Node_no&, const Edge_no&);
    void deleteHost( const Node_no&);
    void print(string, string);

    //GraphPart の種番号だけを変えたものを返す
    GraphPart changeGno(G_no g_no);
    
};

void GraphPart::init2(int h_level, int r_level = 0 ) {
    int h_cnt = 0;
    int s_no = 0;
    Edge::edgeStatus status = Edge::edgeStatus::UNLOCK;
    vector<int> cnts(s, 0);
    set<int> sw_host_nos;


    if ( h_level >= r ) {
        string msg = "[Error]GraphPart::init2() h_level >= number of ports";
        cout << msg << endl;
        throw IregalValueException(msg);
    }

     while(h_cnt < h) {
        switchs[s_no].setEdge(Edge_no(cnts[s_no]), Edge::makeToHost(g, Node_no(h_cnt), status)); 
        hosts[h_cnt].setEdge(Edge::makeToSwitich(g, Node_no(s_no), Edge_no(cnts[s_no]), status)); 
        if ( cnts[s_no] == 0 ) sw_host_nos.insert(s_no);
        ++h_cnt; cnts[s_no]++;
        if ( cnts[s_no] >= h_level ) {
            // for ( ; cnts[s_no] < r; cnts[s_no]+= 1 ) {
            //     switchs[s_no].setEdge(Edge_no(cnts[s_no]), Edge::makeToLoop(g, Node_no(s_no), Edge_no(cnts[s_no])));
            //     cnts[s_no] += 1;
            // }
            s_no += 1; 
        }
        if ( s_no >= s ) {
            string msg = "[Error]GraphPart::init2() not enough switchs for init2()";
            cout << msg << endl;
            throw IregalValueException(msg);
        }
    }

    //ホストとのスイッチをあまりのスイッチに均等に割りふる
    //*********こっから****************

    vector<int> sw_nos; //スイッチ役のスイッチの番号
    vector<int> sw_host; //ホスト役のスイッチの番号

    for ( int i = 0; i < s; ++i ) {
        if ( sw_host_nos.find(i) == sw_host_nos.end() ) sw_nos.push_back(i);
        else sw_host.push_back(i);
    } 

    int sw_idx = 0;
    for ( int i = 0; i < sw_host.size(); ++i ) {
        int node1 = sw_host[i];
        int node2 = sw_nos[sw_idx];
        int edge1 = cnts[node1]; 
        int edge2 = cnts[node2];

        if ( edge1 >= r || edge2 >= r ) {
            string msg = "[Error] GraphPart::init2():: can't make graph";
            throw IregalValueException(msg);
        }

        switchs[node1].setEdge(Edge_no(edge1), Edge::makeToSwitich(G_no(g), Node_no(node2), Edge_no(edge2), status));
        switchs[node2].setEdge(Edge_no(edge2), Edge::makeToSwitich(G_no(g), Node_no(node1), Edge_no(edge1), status));
        cnts[node1] += 1; cnts[node2] += 1; 

        sw_idx = (sw_idx+1)%sw_nos.size();
    }
    
    for ( int i = 0; i < sw_nos.size(); ++i ) {
        int node1 = sw_nos[i];
        if ( cnts[node1] >= r ) continue;
        
        int rnode= sw_nos[(i+1)%sw_nos.size()];
        
        if ( cnts[rnode] < r && cnts[node1] < r ) {
            switchs[node1].setEdge(cnts[node1], Edge::makeToSwitich(g, Node_no(rnode), Edge_no(cnts[rnode]), status));
            switchs[rnode].setEdge(cnts[rnode], Edge::makeToSwitich(g, Node_no(node1), Edge_no(cnts[node1]), status));
            ++cnts[rnode]; ++cnts[node1];
           }
    }
    
    for ( int i = 0; i < s; ++i ) {
        for ( cnts[i]; cnts[i] < r; ++cnts[i] ) {
            switchs[i].setEdge(cnts[i], Edge::makeToLoop(G_no(g), Node_no(i), Edge_no(cnts[i])));
        }
    }
}

//種数1のグラフの生成
void GraphPart::init() {
    DEB(DEB_MIDLE) { cout << "[Log]GraphPart::init()" << endl;}
    if ( s*r <= h ) { cout << "ホスト数が総ポート数以上です" << endl; return; }
    vector<int> cnts(s, 0); //スイッチiの使ってるポート数


    //ホストを均等に割り振り
    for ( int i = 0; i < h; ++i ) {
        int sno = i%s;
        switchs[sno].setEdge(Edge_no(cnts[sno]), Edge::makeToHost(g, Node_no(i)));
        hosts[i].setEdge( Edge::makeToSwitich(g, Node_no(sno), Edge_no(cnts[sno])));
        ++cnts[sno];
     }

    int sum_cnt = 0;
    for ( auto v: cnts ) sum_cnt += v;

    int sno = 0;
    //スイッチ数が1の場合は例外
    while ( sum_cnt < s*r ) {
        if ( cnts[sno] >= r ) { sno = (sno+1)%s; continue; }

        int rno = (sno+1)%s;
        int lno = (sno-1);
        if ( lno < 0 ) lno += s;

        if ( cnts[lno] < r && cnts[sno] < r) {
            switchs[sno].setEdge(cnts[sno], Edge::makeToSwitich(g, Node_no(lno), Edge_no(cnts[lno])));
            switchs[lno].setEdge(cnts[lno], Edge::makeToSwitich(g, Node_no(sno), Edge_no(cnts[sno])));
            ++cnts[sno]; ++cnts[lno];
            sum_cnt += 2;
        }

        if ( cnts[rno] < r && cnts[sno] < r ) {
            switchs[sno].setEdge(cnts[sno], Edge::makeToSwitich(g, Node_no(rno), Edge_no(cnts[rno])));
            switchs[rno].setEdge(cnts[rno], Edge::makeToSwitich(g, Node_no(sno), Edge_no(cnts[sno])));
            ++cnts[rno]; ++cnts[sno];
            sum_cnt += 2;
           }

        //あまりは自己ループにする
        while ( cnts[sno] < r ) {
            switchs[sno].setEdge(cnts[sno], Edge::makeToLoop(g, Node_no(sno), Edge_no(cnts[sno])));
            cnts[sno]++;
            sum_cnt += 1;
        }

        sno = (sno+1)%s;
      }
} 

/*Switch辺以外の正誤判定
*/
bool GraphPart::isRightEdge ( const Edge& edge ) {
    if ( edge.getType() == Edge::NONE ) return false;
    if ( edge.getType() == Edge::HOST ) {
        if ( edge.getG() != this->g ) return false;
        if ( edge.getNode().getNo() >= this->hosts.size() ) return false;
        return true;
    }
    if ( edge.getType() == Edge::LOOP ) {
        if ( edge.getG() != this->g ) return false;
        if ( edge.getNode().getNo() >= this->switchs.size() )return false;
        if ( edge.getEdge().getNo() >= get_switch(edge.getNode()).get_r() ) return false;
        return true;
    }
    return true;
}

Host&  GraphPart::addHost(const Node_no& s_no, const Edge_no& edge_no) { //ホストを追加するスイッチのノード番号とエッジ番号
    const int node_no = s_no.getNo();
    if( node_no < 0 || node_no >= s ) throw IregalValueException("[GraphPart::addHost()] Iregal s_no");
    if( edge_no.getNo() >= switchs[node_no].get_r() ) throw IregalValueException("[GraphPart::addHost()] Iregal edge_no");
    Host host (Edge::makeToSwitich(G_no(g), s_no, edge_no ));
    switchs[node_no].setEdge(edge_no, Edge::makeToHost(g, Node_no(hosts.size())));
    hosts.push_back(host);
    h = hosts.size();
    return hosts.back();
}


 void GraphPart::deleteHost(const Node_no& h_no ) { //削除するホストの番号
    if ( h_no.getNo() >= hosts.size() ) throw IregalManuplateException("[GraphPart::deleteHost()] Ireagal h_no to deleteHost");
    Edge tailedHost = hosts.back().getEdge();
    Edge delHostEdge = get_host(h_no).getEdge();
    Switch& switchBydel = get_switch(delHostEdge.getNode());
    Switch& switchFortail = get_switch(tailedHost.getNode());
    switchFortail.setEdge(tailedHost.getEdge(), Edge::makeToHost(g, h_no));
    switchBydel.setEdge(delHostEdge.getEdge(), Edge::makeToLoop(delHostEdge.getG(), delHostEdge.getNode(), delHostEdge.getEdge()) );
    hosts[h_no.getNo()] = hosts.back();
    hosts.pop_back();
    h = hosts.size();
}

bool GraphPart::isSame(const GraphPart& gp) {
    DEB(DEB_HIGH) { cout << "[MIDLE]GraphPart::isSame()" << endl; }
    if ( s != gp.s ) return false;
    if ( g != gp.g ) return false;
    if ( h != gp.h ) return false;
    if ( r != gp.r ) return false;

    for ( int i = 0; i < s; ++i ) {
        DEB(DEB_MIDLE) { cout << "[MIDLE]GraphPart::isSame switch = " << i << endl;}
        if ( !(switchs[i].isSame(gp.switchs[i]))) return false;
    }

    DEB(DEB_MIDLE) { cout << "[MIDLE]GraphPart::isSame hosts start" << endl;} 
    vector<Edge> vec1, vec2;
    for ( int i = 0; i < h; ++i ) {
        Edge e = hosts[i].getEdge();
        e.setEdge(Edge_no(0));
        vec1.push_back(e);
    }

    for ( int i = 0; i < h; ++i ) {
        Edge e = gp.hosts[i].getEdge();
        e.setEdge(Edge_no(0));
        vec2.push_back(e);
    }
    
    sort(vec1.begin(), vec1.end());
    sort(vec2.begin(), vec2.end());
    
    DEB(DEB_MIDLE) { cout << "[MIDLE]GraphPart::isSame hosts finish" << endl;}
    return vec1 == vec2;
}


void GraphPart::print(string name="GraphPart", string stuff="" ) {
    string tmp = stuff+"   ";
    string arrow = "";
    for ( int i = 0; i < 70; ++i ) arrow += "-";

    cout << stuff << '[' << name << "]<"+arrow << endl;
    cout << tmp << "s=" << s << " h=" << h << " g_no=" << g.getNo() << " r=" << r <<endl;

    for ( int i = 0; i < switchs.size(); ++i ) {
        switchs[i].print("Switch "+to_string(i), tmp);
    }

    cout << endl;
    for ( int i = 0; i < hosts.size(); ++i) {
        hosts[i].print("Host "+to_string(i), tmp);
    }
    cout << stuff << '[' << name << "]"+arrow+">" << endl;
}

//破壊的なので注意
GraphPart GraphPart::changeGno(G_no g_no) {
    GraphPart gp = (*this);

    gp.g = g_no;
    for ( auto& sw : gp.switchs ) {
        for ( int i = 0; i < sw.get_r(); ++i ) {
            Edge& edge = sw.getEdge(Edge_no(i));
            edge.setG(g_no);
        }
    }
    for ( auto& host : gp.hosts ) {
        Edge& edge = host.getEdge();
        edge.setG(g_no);
    }

    return gp;
}

#endif