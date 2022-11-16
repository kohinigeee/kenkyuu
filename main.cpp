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
#include"./header/Annealing.hpp"

#define INF (1LL<<60)

using namespace std;

int main()
{
    debug_off();

    DEB(DEB_HIGH) { cout << "test" << endl;}
    string outf = "before_swing.dot";
    string outf2 = "after_swing.dot";

    try { 
      Graph graph = Graph::make(16, 32, 4, 1);
      mt19937 mt;

      mt.seed(10);
      Edge edge = sel_edge(0, graph, mt);
      edge.print();

    } catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}