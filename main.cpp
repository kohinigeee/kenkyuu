#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<cmath>
#include<cstdio>

#include"./header/Edge.hpp"
#include"./header/Nodes.hpp"
#include"./header/GraphPart.hpp"
#include"./header/Graph.hpp"

#define INF (1LL<<60)

bool debug = true;

using namespace std;

int main()
{
    debug_off();

    DEB() { cout << "test" << endl;}

    try { 
        Graph graph = Graph::make(8, 12, 4, 1);

        graph_info_t ans = graph.sumD();
        Graph::toDot("before_swing.dot", graph);
        cout << "sumd : " << ans["sumd"] << ", diam : " << ans["diam"] << endl;
        cout << ans["v1"] << " - " << ans["v2"] << endl;
    } catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}