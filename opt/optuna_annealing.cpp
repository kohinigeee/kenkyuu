#include<iostream>
#include<random>
#include<algorithm>
#include<string>
#include<cstdio>

#include"../header/Header_include.hpp"

using namespace std;


int main(int argc, char** args ) 
{
    debug_off();
    annealing_log_off_all();

    vector<string> arg;
    for ( int i = 1; i < argc; ++i ) arg.push_back(string(args[i]));
    Params params = Params(arg);

    const int weight_diam = 1000;
    const int s = params.get("s");
    const int h = params.get("h");
    const int r = params.get("r");
    mt19937 seed_gen;

    seed_gen.seed(params.get("seed"));

    try {

        Graph::set_seed(seed_gen());
        params.set("seed", seed_gen());

        Graph graph = Graph::make(s, h, r, 1);
        Graph best = annealing(graph, params, select_edges_noraml);
        graph_info_t info = best.sumD();

        double haspl = best.calcHaspl(info["sumd"]);
        int diam = info["diam"];

        double score = weight_diam*diam + haspl;

        printf("%0.8lf", score);

    } catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}