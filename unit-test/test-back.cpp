#include<fstream>
#include<string>
#include<cmath>
#include<cstdio>

#include"../header/Edge.hpp"
#include"../header/Nodes.hpp"
#include"../header/GraphPart.hpp"
#include"../header/Graph.hpp"
#include"../header/Debug.hpp"

using namespace std;

bool test(Graph& g, Graph& g2, Edge e1, Edge e2 ) {
    cout << "\nFuncton test" << endl;
    e1.print("Edge1");
    e2.print("Edge2");
    if ( !(g.isSame(g2)) ) return false;

    g.toDot("graph_1.dot", g);

    g.print("Graph1");

    g.simple_swing(e1, e2);

    g.print("Graph2");
    cout << "Do back()" << endl;
    g.toDot("graph_2.dot", g);

    g.back();
    g.toDot("graph_3.dot", g);
    g.print("Back");
    if ( !(g.isSame(g2)) ) return false;
    return true;
}

int main()
{
    debug_off();

    try {
        Graph g = Graph::make(9, 13, 4, 1);
        Graph g2 = g;

        // g.print("Graph");

        Edge e1, e2;

        //スイッチ辺二つのパターン
        e1 = g.getSwitch(G_no(0), Node_no(0)).getEdge(Edge_no(2));
        e2 = g.getSwitch(G_no(0), Node_no(4)).getEdge(Edge_no(2));
        if ( test(g, g2, e1, e2) ) cout << "E2L0H0 OK" << endl;
        else { cout << "E2L0H0 OUT" << endl; return 0; }

       //ホスト辺1つのパターン 
       e1 = g.getSwitch(G_no(0), Node_no(1)).getEdge(Edge_no(1));
       e2 = g.getSwitch(G_no(0), Node_no(4)).getEdge(Edge_no(2));
       if ( test(g, g2, e1, e2) ) cout << "E1L0H1 OK" << endl;
       else { cout << "E1L0H1 OUT" << endl; return 0;}

    }  catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    }  catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}