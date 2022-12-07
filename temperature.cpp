#include<iostream>
#include<vector>
#include<utility>
#include<cstdio>

#include"./header/Header_include.hpp"

using namespace std;

int main(int argc, char**args)
{
    debug_off();
    annealing_log_off(0);

    vector<string> arg;
    for ( int i = 1; i < argc; ++i ) arg.push_back(string(args[i]));

    const int s = 70, h = 80, r = 6, g = 1;
    const int cycle = 100;
    Params params = Params(arg);
    mt19937 mt;
    mt.seed(params.get("seed")); 

    vector<pair<int,int>> tmps;
    vector<string> names;
    vector<double> haspls;
    vector<long long> diams;
    vector<long long> mindiam;

    const long long N = params.get("N");
    tmps.push_back(make_pair(10,100));
    tmps.push_back(make_pair(100,1000));
    tmps.push_back(make_pair(50, 5000));
    tmps.push_back(make_pair(N*0.1, N*10));
    tmps.push_back(make_pair(N*0.1, N*2));

    names.push_back("10-100");
    names.push_back("100-1000");
    names.push_back("50-5000");
    names.push_back("*0.1 - *10.0");
    names.push_back("*0.1 - *2.0");

    mindiam = vector<long long>(tmps.size(), (1LL<<40));
    diams = vector<long long>(tmps.size(), 0);
    haspls = vector<double>(tmps.size(), 0);

    try {
       for ( int i = 0; i < cycle; ++i ) {
        cout << "\ri = " << i ;
        cout << flush;
        int seed = mt();
        Graph::set_seed(seed);

        Graph graph = Graph::make(s, h, r, g);
        for ( int j = 0; j < tmps.size(); ++j ) {
            Params tmp_params = params;
            tmp_params.set("minT", tmps[j].first);
            tmp_params.set("maxT", tmps[j].second);
            tmp_params.set("seed", seed);

            Graph tmp = graph;
            Graph best = annealing(tmp, tmp_params, select_edges_hl);
            graph_info_t best_info = best.sumD();

            haspls[j] += best.calcHaspl(best_info["sumd"]);
            diams[j] += best_info["diam"];
            mindiam[j] = min(mindiam[j], best_info["diam"]);
        }
       } 

       cout << "\r[Results]" << endl;
       cout << "N = " << N << endl;
       cout << "seed = " << params.get("seed") << endl;
       for ( int j = 0; j < tmps.size(); ++j ) {
        double avghaspls = haspls[j]/cycle;
        double avgdiam = double(diams[j])/cycle;
        char name[256];

        stochars(names[j], name);
        printf("%15s :", name); 
        printf(" avg-hasple= %.5lf", avghaspls);
        printf(" avg-diam= %0.5lf", avgdiam);
        printf(" mindiam= %ld\n", mindiam[j]);
       }
    } catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}