#include<iostream>
#include<fstream>

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
    
    const int s = 25;
    const int h = 32;
    const int r = 4;
    const int n = 10;
    const int range = 5;

    vector<int> s_values;
    vector<Results> results1, results2, results3;

    for ( int i = 0; i <= range; ++i ) {
        int tmp_s = s+i;
        s_values.push_back(tmp_s);
        results1.push_back(Results("make_graph_random"+to_string(tmp_s)));
        results2.push_back(Results("make_graph_init"+to_string(tmp_s)));
        results3.push_back(Results("make_graph_mdst"+to_string(tmp_s)));
    }

    for ( int i = 0; i < s_values.size(); ++i ) {
        cout << "\rs = " << s_values[i] << flush;
        const int tmp_s = s_values[i];
        Graph::set_seed(seed_gen());

        Graph random = Graph::makeRandom(s_values[i], h, r);
        Graph init = Graph::make(s_values[i], h, r, 1);
        Graph mdst = init;
        Graph best_mdst = mdst;

        graph_info_t best_mdst_info = best_mdst.sumD();

        for ( int _ = 0; _ < n; ++_ ) {
            Graph::set_seed(seed_gen());
            Params tmp_param = param;
            tmp_param.set("seed", seed_gen());
            

            Graph best1 = annealing(random, tmp_param, select_edges_noraml);
            Graph best2 = annealing(init, tmp_param, select_edges_noraml);
            Graph best3 = annealing(mdst, tmp_param, select_edges_noraml);

            results1[i].add_result(best1);
            results2[i].add_result(best2);
            results3[i].add_result(best3);

            graph_info_t best3_info = best3.sumD();
            if ( Graph::compare(best3, best3_info, best_mdst, best_mdst_info) ) {
                best_mdst = best3;
                best_mdst_info = best3_info;
            }

            random = Graph::makeRandom(s_values[i], h, r);
            init = Graph::make(s_values[i], h, r, 1);
            mdst = makeMDTgraph(best_mdst);
            mdst.linkLoops();   
        }
    }

    for ( int i = 0; i < s_values.size(); ++i ) {
        cout << endl;
        results1[i].print();
        cout << endl;
        results2[i].print();
        cout << endl;
        results3[i].print();
    }
    
    string fname = "./python/results/results_mdst_1.txt";

    ofstream ofs(fname);
    if ( !ofs ) {
        cout << "[" << fname << "]" << "が開けませんでした";
        cin.get();
        return 0;
    }

    char tmps[256];
    sprintf(tmps, "s: %d h: %d r: %d", s, h, r);
    ofs << tmps << endl;
    sprintf(tmps, "N:%d minT:%d maxT:%d seed:%d", param.get("N"), param.get("mintT"), param.get("maxT"), param.get("seed"));
    ofs << tmps << endl;
    sprintf(tmps, "n(number of annealings) %d range %d", n, range);
    ofs << tmps << endl;

    ofs << endl;
    ofs << "avg_haspl avg_diam min_haspl min_diam" << endl;

    for ( int i = 0; i < s_values.size(); ++i ) {
        ofs << s_values[i] << endl;;
        ofs << results1[i].get_avg_haspl() << " " << results1[i].get_avg_diam() << " " << results1[i].get_min_haspl() << " " << results1[i].get_min_diam() << endl; 
        ofs << results2[i].get_avg_haspl() << " " << results2[i].get_avg_diam() << " " << results2[i].get_min_haspl() << " " << results2[i].get_min_diam() << endl; 
        ofs << results3[i].get_avg_haspl() << " " << results3[i].get_avg_diam() << " " << results3[i].get_min_haspl() << " " << results3[i].get_min_diam() << endl;
    }

    cout << fname << "に入力しました" << endl;
}