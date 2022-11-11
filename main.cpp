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


using namespace std;

int main()
{
    debug_off();

    DEB() { cout << "test" << endl;}
    string outf = "before_swing.dot";
    string outf2 = "after_swing.dot";

    try { 
        Graph graph = Graph::make(8, 12, 4, 1);
        Graph graph2 = graph;

        graph.toDot(outf, graph);
        // graph.print("Graph-1");

        Edge e1 = graph.getSwitch(G_no(0), Node_no(0)).getEdge(Edge_no(3));
        Edge e2 = graph.getSwitch(G_no(0), Node_no(4)).getEdge(Edge_no(2)); 

        if ( graph == graph2 ) cout << "Same graph" << endl;
        else cout << "Dont same graph" << endl;

        e1.print("Edge-A");
        e2.print("Edge-B");

        graph.simple_swing(e1, e2);
        graph.toDot(outf2, graph);
        // graph.print("After");

        // graph.print("Graph");
        // graph2.print("Graph2");

        if ( graph == graph2 ) cout << "Same graph" << endl;
        else cout << "Dont same graph" << endl;

        if ( graph.back() ) { cout << "Backed edges" << endl; }
        else { cout << "Couldn't back" << endl; }

        // graph.toDot("graph3.dot", graph);
        if ( graph == graph2 ) cout << "Same graph" << endl;
        else cout << "Dont same graph" << endl;

    } catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}