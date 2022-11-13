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

#define NOTGRAPH1 1
#define NOTSAME 2
#define OK 3
#define NOTGRAPH2 4

using namespace std;

int test(Graph& g, Graph& g2, Edge e1, Edge e2 ) {
    cout << "\nFuncton test" << endl;
    e1.print("Edge1");
    e2.print("Edge2");
    cout << "GRAPH1 PROOP" << endl;
    if ( !(g.isPropGraph()) ) return NOTGRAPH1;
    cout << "GRAPH2 PROOP" << endl;
    if ( !(g2.isPropGraph() ) ) return NOTGRAPH2;
    cout << "IS SAME" << endl;
    if ( !(g.isSame(g2)) ) return NOTSAME;

    cout << "switching" << endl;
    g.simple_swing(e1, e2);
    // g.toDot("graph_2.dot", g);
    // g.print("switchde Graph");
    cout << "after switching" << endl;

    if ( !g.isPropGraph()) return NOTGRAPH1;

    cout << "Do back()" << endl;
    g.back();
    // g.toDot("graph_3.dot", g);
    // g.print("backed Graph");
    if ( !(g.isPropGraph()) ) return NOTGRAPH1;
    if ( !(g2.isPropGraph() ) ) return NOTGRAPH2;
    if ( !(g.isSame(g2)) ) return NOTSAME;
    return OK;
}

int main()
{
    debug_off();
    vector<string> results(5);
    results[NOTGRAPH1] = "NOT GRAPH1";
    results[NOTSAME] = "NOT SAME";
    results[OK] = "OK";
    results[NOTGRAPH2] = "NOT GRAPH2";


    try {
        Graph g = Graph::make(100, 150, 4, 1);
        Graph g2 = g;
        Graph g3 = g;


        g.toDot("graph_1.dot", g);
        // g.print("Graph1");
        mt19937 mt;
        mt.seed(20);

        for ( int i = 0; i < 100; ++i ) {
            cout << "\ni = " << i << endl;
            g3 = g2;
            g2.toDot("graph_2.dot", g2);
            pair<Edge, Edge> p = select_edges(g2, mt);
            p.first.print("A1");
            g2.getEdge(p.first).print("A2");
            p.second.print("B1");
            g2.getEdge(p.second).print("B2");

            int result = test(g2, g3, p.first, p.second);
            cout << "i = " << i << " : " << results[result] << endl;
            if ( result != OK ) {
                cout << "Find Invalid" << endl;
                g2.toDot("graph_2.dot", g2);
                return 0;
            }
            pair<Edge, Edge> p2 = select_edges(g2, mt);
            g2.simple_swing(p2.first, p2.second);
            if ( !(g2.isPropGraph()) ) {
                cout << "Invalid Switching" << endl;
                return 0;
            }
            cout << "i = " << i << " switched" << endl;
        }
        cout << "All Completed" << endl;
        g2.toDot("graph_2.dot", g2);


    }  catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    }  catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}