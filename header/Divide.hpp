#ifndef INCLUDE_DEVIDE_HPP
#define INCLUDE_DEVIDE_HPP

#include<vector>
#include<cmath>

#include"DivideAnnealing.hpp"

using namespace std;

class divideInfo {
    public:
    int r, h, s;
    vector<int> nodes, hosts, ports;

    private:
    divideInfo( const int s, const int h, const int r, vector<int>& nodes, vector<int>& hosts, vector<int>& ports ) : r(r), h(h), s(s), nodes(nodes), hosts(hosts), ports(ports) {} 

    public:
    divideInfo static makeDivide(const int s, const int h, const int r );
};

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

#endif
