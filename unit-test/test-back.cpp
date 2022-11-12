#include<fstream>
#include<string>
#include<cmath>
#include<cstdio>

#include"../header/Edge.hpp"
#include"../header/Nodes.hpp"
#include"../header/GraphPart.hpp"
#include"../header/Graph.hpp"
#include"../header/Debug.hpp"

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

    cout << "Do back()" << endl;
    g.back();
    if ( !(g.isPropGraph()) || !(g2.isPropGraph()) ) return NOTGRAPH;
    if ( !(g.isSame(g2)) ) return NOTSAME;
    return OK;
}

int main()
{
    debug_off();
    vector<string> results(4);
    results[NOTGRAPH] = "NOTGRAPH";
    results[NOTSAME] = "NOTSAME";
    results[OK] = "OK";


    try {
        Graph g = Graph::make(9, 13, 4, 1);
        Graph g2 = g;

        Edge e1, e2;

        //スイッチ辺二つのパターン
        e1 = g.getSwitch(G_no(0), Node_no(0)).getEdge(Edge_no(2));
        e2 = g.getSwitch(G_no(0), Node_no(4)).getEdge(Edge_no(2));
        int result = test(g2, g, e1, e2);
        cout << "Test::E2L0H0 = " << results[result] << endl;

       //ホスト辺1つのパターン 
       g2 = g;
       e1 = g.getSwitch(G_no(0), Node_no(1)).getEdge(Edge_no(1));
       e2 = g.getSwitch(G_no(0), Node_no(4)).getEdge(Edge_no(2));
       result = test(g2, g, e1, e2);
       cout << "Test::E1L0H1 = " << results[result] << endl;

       //スイッチ一つ、ループ1つのパターン

    }  catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    }  catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}