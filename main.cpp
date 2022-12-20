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
    annealing_log_off_all();

    vector<string> arg;
    for ( int i = 1; i < argc; ++i ) arg.push_back(string(args[i]));

    Params params = Params(arg);
    const int s = 32, h = 32, r = 4, g = 1;


    try { 
        mt19937 seed_gen;
        seed_gen.seed(params.get("seed"));

        for ( int i = 0; i < 100; ++i ) {
            Graph::set_seed(seed_gen());
            params.set("seed", seed_gen());
            Graph graph = Graph::make(s, h, r, 1);


            Graph best = annealing_kick(graph, params, select_edges_noraml);
            graph_info_t info =best.sumD();

            cout << "\n" << "results" << endl;
            params.print();
            cout << "diam = " << info["diam"] << endl;
            cout << "haspl = " << best.calcHaspl(info["sumd"]); 
            cout << endl;
        }
    } catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}