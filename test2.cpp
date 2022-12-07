#include<iostream>
#include<random>
#include<algorithm>
#include<string>
#include<cstdio>

#include"header/Header_include.hpp"

using namespace std;


int main(int argc, char** args ) 
{
    debug_off();
    annealing_log_off_all();
    annealing_log_on(0);

    const int s = 32, h = 32, r = 4, g = 1;
    int groups = int(sqrt(s)); //グループの数
    if ( groups*groups < s ) ++groups;
    vector<pair<int,int>> values;
    vector<int> ports;

    cout << "groups = " << groups << endl;
    values = vector<pair<int,int>>(groups, make_pair(s/groups, h/groups));
    ports = vector<int>(groups, 3);
    vector<GraphPart> gps;

    for ( int i = 0; i < s%groups; ++i ) values[i].first += 1;
    for ( int i = 0; i < h%groups; ++i ) values[i].second += 1;

    try {
        Params parmas = Params();
        Graph graph1 = Graph::make(5500, 10000, 8, 1);
        mt19937 mt;
        Graph best = annealing(graph1, parmas, select_edges_noraml);
        best.toDot("graph2.dot", best);

        graph_info_t info = best.sumD();
        double haspl = best.calcHaspl(info["sumd"]);
        cout << "haspl = " << haspl << endl;
        cout << "diam = " << info["diam"] << endl;
    } catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}