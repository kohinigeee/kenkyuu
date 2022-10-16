#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<cmath>

#include"./header/Edge.hpp"
#include"./header/Nodes.hpp"
#include"./header/GraphPart.hpp"
#include"./header/Graph.hpp"

bool debug = false;

using namespace std;

int main()
{
    debug_off();

    Graph graph = Graph::make(9, 12, 4, 3);
    graph.print("Before-Swing");
    Graph::toDot("before_swing.dot", graph);

    try { 
        Edge a1 = graph.getEdge(Edge::SWITCH, G_no(0), Node_no(0), Edge_no(3));

        Edge b1 = graph.getEdge(Edge::SWITCH, G_no(1), Node_no(1), Edge_no(2));
        graph.swing(a1, b1);

    } catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
        Graph::toDot("after_swing.dot", graph);
    }
    Graph::toDot("after_swing.dot", graph);
    graph.print("After-Swing");
}