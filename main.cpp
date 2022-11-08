#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<cmath>

#include"./header/Edge.hpp"
#include"./header/Nodes.hpp"
#include"./header/GraphPart.hpp"
#include"./header/Graph.hpp"

#define INF (1LL<<60)

bool debug = true;

using namespace std;

int main()
{
    debug_on();

    DEB() { cout << "test" << endl;}

    try { 
        Graph graph = Graph::make(8, 12, 4, 1);
        vector<vector<long long>> d = graph.calcBFS(-1);

        for ( auto vec : d ) {
            for ( auto v : vec ) cout << v << " "; cout << endl;
        }
        // Edge e1 = graph.getEdge(Edge::edgeType::SWITCH, G_no(0), Node_no(0), Edge_no(0) );
        // Edge e2 = graph.getEdge(Edge::edgeType::SWITCH, G_no(0), Node_no(2), Edge_no(2) );
        Graph::toDot("before_swing.dot", graph);
        graph.print("Before-Swing");
        // graph.simple_swing(e1, e2);
        // Graph::toDot("after_swing.dot", graph);

    } catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}