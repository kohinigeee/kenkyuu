#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<cmath>
#include<cstdio>

#include "./header/Header_include.hpp"

#define INF (1LL<<60)

using namespace std;

int main(int argc, char** args)
{
    debug_off();
    annealing_log_off(0);

    vector<string> arg;
    for ( int i = 1; i < argc; ++i ) arg.push_back(string(args[i]));

    Params params = Params(arg);
    const int s = 28, h = 32, r = 4, g = 1;


    try { 
      Graph::set_seed(params.get("seed"));
      Graph graph = Graph::makeRandom(s, h, r); 

      Graph::toDot("graph1.dot", graph, 5);
      graph.print();

      Graph best = annealing(graph, params, select_edges3);
      graph_info_t info =best.sumD();

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