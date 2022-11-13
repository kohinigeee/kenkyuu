#include<fstream>
#include<string>
#include<cmath>
#include<cstdio>
#include<random>

#include"../header/Edge.hpp"
#include"../header/Nodes.hpp"
#include"../header/GraphPart.hpp"
#include"../header/Graph.hpp"
#include"../header/Debug.hpp"
#include"../header/Annealing.hpp"

#define NOTGRAPH 1
#define NOTSAME 2
#define OK 3

using namespace std;

int test(Graph& g, Graph& g2, Edge e1, Edge e2 ) {
    cout << "\nFuncton test" << endl;
    e1.print("Edge1");
    e2.print("Edge2");
    if ( !(g.isPropGraph()) || !(g2.isPropGraph()) ) return NOTGRAPH;
    if ( !(g.isSame(g2)) ) return NOTSAME;

    g.simple_swing(e1, e2);
    // g.toDot("graph_2.dot", g);
    // g.print("switchde Graph");

    if ( !g.isPropGraph()) return NOTGRAPH;

    cout << "Do back()" << endl;
    g.back();
    // g.toDot("graph_3.dot", g);
    // g.print("backed Graph");
    if ( !(g.isPropGraph()) || !(g2.isPropGraph()) ) return NOTGRAPH;
    if ( !(g.isSame(g2)) ) return NOTSAME;
    return OK;
}

int main()
{
    debug_off();
    vector<string> results(4);
    results[NOTGRAPH] = "NOT GRAPH";
    results[NOTSAME] = "NOT SAME";
    results[OK] = "OK";


    try {
        Graph g = Graph::make(9, 13, 4, 1);
        Graph g2 = g;


        // g.toDot("graph_1.dot", g);
        // g.print("Graph1");
        mt19937 mt;
        mt.seed(5);

        for ( int i = 0; i < 10; ++i ) {
        pair<Edge, Edge> p = select_edges(g, mt);

        cout << "\ni = " << i << endl;
        Edge e1 = g.getEdge(p.first);
        Edge e2 = g.getEdge(p.second);

        p.first.print("A1");
        e1.print("A2");
        cout << endl;
        p.second.print("B1");
        e2.print("B2");
    }


    }  catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    }  catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}