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

    Graph graph(6, 8, 3, 2);

    // graph.print();

    Edge a1 = graph.getEdge(SWITCH, 0, 0, 0);
    Edge b1 = graph.getEdge(SWITCH, 0, 2, 1);

    graph.print("Before-Swing");
    Graph::toDot("before_swing.dot", graph);

    // graph.swing(a1, b1);
    // a1 = graph.getEdge(SWITCH, 1, 2, 2);
    // b1 = graph.getEdge(SWITCH, 0, 1, 1);
    graph.swing(a1, b1);
    Graph::toDot("after_swing.dot", graph);
    graph.print("After-Swing");
    // Edge a_to = graph.getEdge(a1);
    // cout << "a1 = "; a1.print();
    // cout << "a_to = "; a_to.print();
}