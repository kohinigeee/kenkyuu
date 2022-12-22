#ifndef INCLUDE_DEVIDE_HPP
#define INCLUDE_DEVIDE_HPP

#include<vector>
#include<cmath>
#include<map>

using namespace std;

vector<int> divideHosts(const int r, const int h, const vector<int>& nodes );

class divideInfo {
    public:
    int r, h, s;
    vector<int> nodes, hosts, ports;

    private:
    divideInfo( const int s, const int h, const int r, vector<int>& nodes, vector<int>& hosts, vector<int>& ports ) : r(r), h(h), s(s), nodes(nodes), hosts(hosts), ports(ports) {} 

    public:
    void print() {
        cout << "[divideInfo]" << endl;
        cout << "s = " << s << ", h = " << h << ", r = " << r << endl;

        for ( auto v : nodes ) cout << v << " "; cout << endl;
        for ( auto v : hosts) cout << v << " "; cout << endl;
        for ( auto v : ports ) cout << v << " "; cout << endl;
    }
    divideInfo static makeDivide(const int s, const int h, const int r );
    divideInfo static makeDivide_normal( const int s, const int h, const int r);
    int get_size() const { return nodes.size(); }
};

//swithcs: √s
//hosts: 1個ずつ均等に配布
//ports: r-1
divideInfo divideInfo::makeDivide_normal( const int s, const int h, const int r ) {
    vector<int> nodes, hosts, ports;
    int gropus = sqrt(s);
    if ( gropus*gropus < s ) ++gropus;

    nodes = vector<int>(gropus, s/gropus);
    for ( int i = 0; i < s%gropus; ++i ) nodes[i] += 1;

    hosts = vector<int>(gropus, h/gropus);
    for ( int i = 0; i < h%gropus; ++i ) hosts[i] += 1;
    ports = vector<int>(gropus, r-1);

    for ( int i = 0; i < gropus; ++i ) {
        if ( nodes[i]*r <= hosts[i]+ports[i] ) {
            cout << "[Error] divideInfo::makeDivide made invalid information" << endl;
            exit(1);
        }
    }

    return divideInfo(s, h, r, nodes, hosts, ports);
}

//swithces: √s
//hosts: 占有率 min-max sub
//ports: r-1
divideInfo divideInfo::makeDivide ( const int s, const int h, const int r ) {
    vector<int> nodes, hosts, ports;

    int groups = sqrt(s);
    if ( groups*groups < s ) ++groups;

    nodes = vector<int>(groups, s/groups);
    for ( int i = 0; i < s%groups; ++i ) nodes[i] += 1;

    try {
        hosts = divideHosts(r, h, nodes);
    } catch ( IregalValueException e ) {
        exit(1);
    }

    ports = vector<int>(groups, r-1);

    for ( int i = 0; i < groups; ++i ) {
        if ( nodes[i]*r <= hosts[i]+ports[i] ) {
            cout << "[Error] divideInfo::makeDivide made invalid information" << endl;
            exit(1);
        }
    }

    return divideInfo(s, h, r, nodes, hosts, ports);
}


// 分割探索用のホスト分割(占有率min maxdif ver)
// @gropus : グループ数
// @nodes : 各グループの振り分けスイッチ数
// [message] : nodes内の値は2種類のみ前提
// [注意」割り振り全探索時に、実行可能性を未判定
vector<int> divideHosts(const int r, const int h, const vector<int>& nodes )  {
    map<int, int> cnts;
    const int gropus = nodes.size();

    for ( auto v : nodes ) {
        if ( cnts[v] == 0 ) cnts[v] = 1;
        else cnts[v] += 1;
    }

    if ( cnts.size() <= 0 || cnts.size() >= 3 ) {
        string msg = "[Error] divideHosts:: nodes.size is not invalid";
        cout << msg << endl;
        throw IregalValueException(msg);
    }

    vector<pair<int,int>> spe_nodes;
    for ( auto pii : cnts ) spe_nodes.push_back(pii);

    if ( cnts.size() == 1 ) {
        vector<int> ans = vector<int>(gropus,h/gropus);
        for ( int i = 0; i < h%gropus; ++i ) ans[i] += 1;
        return ans;
    }

    vector<int> ans;
    pair<int,int> best_hsize = make_pair(-1,-1);
    Rational bestratio = Rational((1LL<<60), 1);
    
    for ( int i = 0; i <= h; ++i ) {
        pair<int,int> h_size = make_pair(i, h-i);
        int noOfNodes1 = spe_nodes[0].first; //グループの担当スイッチ数
        int noOfGroups1 = spe_nodes[0].second; //担当スイッチ数が noOfNodes1 と等しいグループの数
        int noOfNodes2 = spe_nodes[1].first;
        int noOfGropus2 = spe_nodes[1].second;

        int amari = ( h_size.first%noOfGroups1 == 0 ) ? 0 : 1;
        Rational min1 = Rational(h_size.first/noOfGroups1, noOfNodes1*r);
        Rational max1 = Rational(h_size.first/noOfGroups1+amari, noOfNodes1*r);

        amari = ( h_size.second%noOfGropus2 == 0 ) ? 0 : 1;
        Rational min2 = Rational(h_size.second/noOfGropus2, noOfNodes2*r);
        Rational max2 = Rational(h_size.second/noOfGropus2+amari, noOfNodes2*r);

        Rational tmp = max(max1, max2)-min(min1, min2);

        // cout << endl;
        // cout << "h_size.second = " << h_size.second << ", " << "noOfGroups2 = " << noOfGropus2 << endl;
        // cout << "amari = " << amari << endl;
        // cout << "min1 = " << min1 << ", max1 = " << max1 << endl;
        // cout << "min2 ="  << min2 << ", max2 = " << max2 << endl;
        // cout << "h_size = " << h_size.first << ", " << h_size.second << " : tmp = " << tmp << endl;
        if ( tmp < bestratio ) {
            best_hsize = h_size;
            bestratio = tmp;
        }
    }

    if ( best_hsize == make_pair(-1, -1) ) {
        return ans;
    }

    // cout << "best_hsize = " << best_hsize.first << ", " << best_hsize.second << endl;
    ans = vector<int>(gropus, 0);
    pair<int,int> cnt = make_pair(0,0);
    for ( int i = 0; i < gropus; ++i ) {
        if ( nodes[i] == spe_nodes[0].first ) {
            ans[i] = best_hsize.first/spe_nodes[0].second;
            if ( cnt.first < best_hsize.first%spe_nodes[0].second ) {
                ans[i] += 1;
                cnt.first += 1;
            }
        } else {
            ans[i] = best_hsize.second/spe_nodes[1].second;
            if ( cnt.second < best_hsize.second%spe_nodes[1].second ) {
                ans[i] += 1;
                cnt.second += 1;
            }
        }
    }
    return ans;
}
#endif
