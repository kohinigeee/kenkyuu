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
    annealing_log_off(0);

    DEB(DEB_HIGH) { cout << "test" << endl;}
    string outf = "before_swing.dot";
    string outf2 = "after_swing.dot";

    try { 
      Graph graph = Graph::make(27, 32, 4, 1);
      mt19937 mt;
      Params params = Params();
      params.set("N", 4000);
      mt.seed(12);

      Graph best = annealingWithMDST(200, graph, params, select_edges3, mt);
    //   Graph best = annealing(graph, params, select_edges3);
      graph_info_t info = best.sumD();

      best.toDot("graph2.dot", best, 5);
      cout << "\n" << "results" << endl;
      params.print();
      cout << "diam = " << info["diam"] << endl;
      cout << "haspl = " << best.calcHaspl(info["sumd"]); 

    } catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}