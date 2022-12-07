#include<iostream>

#include"./header/Header_include.hpp"

using namespace std;

int main(int argc, char** args)
{
    debug_off();
    annealing_log_off_all();

    vector<string> arg; 
    for ( int i = 1; i < argc; ++i ) {
        arg.push_back(string(args[i]));
    }

    Params param = Params(arg);
    int seed = param.get("seed");
    double minTa = 0.1;
    double maxTa = 10.0;

    mt19937 seed_gen;
    seed_gen.seed(seed);

    param.set("minT", param.get("N")*minTa);
    param.set("maxT", param.get("N")*maxTa);
    
    const int s = 10;
    const int h = 128;
    const int r = 24;
    const int n = 100;
    const int range = 10;

    vector<int> s_values;
    vector<Results> results1, results2;

    for ( int i = 0; i <= range; ++i ) {
        int tmp_s = s+i;
        s_values.push_back(tmp_s);
        results1.push_back(Results("select_edges_normal_"+to_string(tmp_s)));
        results2.push_back(Results("select_edges_hl_"+to_string(tmp_s)));
    }

    for ( int i = 0; i < s_values.size(); ++i ) {
        cout << "s = " << s_values[i] << endl;
        const int tmp_s = s_values[i];

        for ( int _ = 0; _ < n; ++_ ) {
            Graph::set_seed(seed_gen());
            Params tmp_param = param;
            tmp_param.set("seed", seed_gen());

            Graph graph = Graph::make(s_values[i], h, r, 1);
            Graph graph2 = graph;


            Graph best1 = annealing(graph, tmp_param, select_edges_noraml);
            Graph best2 = annealing(graph2, tmp_param, select_edges_hl);

            results1[i].add_result(best1);
            results2[i].add_result(best2);
        }
    }
    
    for ( int i = 0; i < s_values.size(); ++i ) {
        cout << endl;
        results1[i].print();
        cout << endl;
        results2[i].print();
    }
}