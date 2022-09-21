#ifndef INCLUDE_GRAPHPART_HPP
#define INCLUDE_GRAPHPART_HPP

#include "Nodes.hpp"
#include <iostream>
#include <vector>

//一つの種を表すクラス
class GraphPart {
    int s, h, g; //s : スイッチ数, h:ホスト数, g:種の番号
    int r;
    vector<Switch> switchs;
    vector<Host> hosts;

    public:
    GraphPart(int s1, int h1, int r1, int g1 = 0) : s(s1), h(h1), r(r1), g(g1) {
    // s_start : スイッチの絶対番号の最初の番号
    // h_start : ホストの絶対番号の最初の番号

        for ( int i = 0; i < s1; ++i ) {
            switchs.push_back(Switch(r));
        }
        for ( int i = 0; i < h1; ++i ) hosts.push_back(Host());
    }

    inline int get_hsize() const { return h; }
    inline int get_ssize() const { return s; }
    inline int get_gno() const { return g; }
    inline int get_r() const { return r; }

    inline void set_gno( int g_no ) { g = g_no; }

    inline Host& get_host(int no) { return hosts[no];}
    inline const Host& get_host(int no ) const { return hosts[no]; }

    inline Switch& get_switch(int no ) { return switchs[no]; }
    inline const Switch& get_switch(int no ) const { return switchs[no]; }

    inline const vector<Switch>& get_switchs() const { return switchs; }
    inline const vector<Host>& get_hosts() const { return hosts; }
    inline vector<Host>& get_hosts() { return hosts; }

    GraphPart clone(int g_no) {
        GraphPart gp = (*this);
        for ( int i = 0; i < s; ++i ) {
            Switch& sw = gp.get_switch(i);
            for ( int j = 0; j < sw.get_r(); ++j ) {
                sw.getEdge(j).to_g = g_no;
            }
        }

        for ( int i = 0; i < h; ++i ) {
        gp.get_host(i).getEdge().to_g = g_no;
        }
        gp.set_gno(g_no);
        return gp;
    }

    GraphPart clone() { return (*this).clone(g+1); }

    GraphPart& operator= (const GraphPart& gp ) {
        s = gp.get_ssize(); h = gp.get_hsize(); g = gp.get_gno();
        r = gp.get_r();
        switchs = gp.get_switchs();
        hosts = gp.get_hosts();
        return *this;
    }

    
    //種数1のグラフの生成
    void init();
    void print(string, string);
};

//種数1のグラフの生成
void GraphPart::init() {
    if ( s*r <= h ) { cout << "ホスト数が総ポート数以上です" << endl;return; }
    vector<int> cnts(s, 0);

    //ホストを均等に割り振り
    for ( int i = 0; i < h; ++i ) {
        int sno = i%s;
        switchs[sno].setEdge(cnts[sno], Edge(HOST, g, i, -1));
        hosts[i].setEdge( Edge(SWITCH, g, sno, cnts[sno]));
        ++cnts[sno];
     }


     int sum_cnt = 0;
    for ( auto v: cnts ) sum_cnt += v;

    int sno = 0;

    //スイッチ数が1の場合は例外
    while ( sum_cnt < s*r ) {
        int rno = (sno+1)%s;
        if ( cnts[sno] >= r ) { sno = (sno+1)%s; continue; }

        if ( cnts[rno] < r ) {
            switchs[sno].setEdge(cnts[sno], Edge(SWITCH, g, rno, cnts[rno]));
               switchs[rno].setEdge(cnts[rno], Edge(SWITCH, g, sno, cnts[sno]));
            ++cnts[rno]; ++cnts[sno];
                sum_cnt += 2;
           }

        int lno = (sno-1);
        if ( lno < 0 ) sno += s;

        if ( cnts[lno] < r ) {
                switchs[sno].setEdge(cnts[sno], Edge(SWITCH, g, lno, cnts[lno]));
            switchs[lno].setEdge(cnts[lno], Edge(SWITCH, g, sno, cnts[sno]));
            ++cnts[lno]; ++cnts[sno];
            sum_cnt += 2;
        }

        //あまりは自己ループにする
        while ( cnts[sno] < r ) {
            switchs[sno].setEdge(cnts[sno], Edge(LOOP, g, sno, cnts[sno]));
            cnts[sno]++;
            sum_cnt += 1;
        }

        sno = (sno+1)%s;
      }
}

void GraphPart::print(string name="GraphPart", string stuff="" ) {
    string tmp = stuff+"   ";
    string arrow = "";
    for ( int i = 0; i < 70; ++i ) arrow += "-";

    cout << stuff << '[' << name << "]<"+arrow << endl;
    cout << tmp << "s=" << s << " h=" << h << " g_no=" << g << " r=" << r <<endl;

    for ( int i = 0; i < switchs.size(); ++i ) {
        switchs[i].print("Switch "+to_string(i), tmp);
    }

    cout << endl;
    for ( int i = 0; i < hosts.size(); ++i) {
        hosts[i].print("Host "+to_string(i), tmp);
    }
    cout << stuff << '[' << name << "]"+arrow+">" << endl;
}

#endif