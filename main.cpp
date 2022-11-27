#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<cmath>
#include<cstdio>

#include "./header/Header_include.hpp"

#define INF (1LL<<60)

using namespace std;

int main()
{
    debug_off();
    annealing_log_off();

    DEB(DEB_HIGH) { cout << "test" << endl;}
    string outf = "before_swing.dot";
    string outf2 = "after_swing.dot";

    try { 
      Graph graph = Graph::make(20, 32, 4, 1);
      mt19937 mt;
      Params params = Params();
      mt.seed(10);

      Graph best = annealing(graph, params, select_edges3);
      graph_info_t info = graph.sumD();

      best.toDot("graph1.dot", best, 5);
      cout << "diam = " << info["diam"] << endl; 

      Graph tree = makeMDTgraph(best);

      tree.toDot("graph2.dot", tree, 5);
      tree.print("Tree");

      info = tree.sumD();
      cout << "diam = " << info["diam"] << endl; 
      cout << "points = " << info["v1"] << ", " << info["v2"] << endl;

      tree.linkLoops();

      tree.toDot("graph3.dot", tree, 5);

    } catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}