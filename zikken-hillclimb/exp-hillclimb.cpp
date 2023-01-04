#include<iostream>
#include<vector>

#include"../header/Header_include.hpp"
#include"ClimbParams.hpp"

using namespace std;

void output(string, ClimbParams&, vector<Results>&);

void exp_hillclimb(string path, ClimbParams& params ) {
    mt19937 seed_gen;
    seed_gen.seed(params.seed);
 
    vector<Results> results(params.range+1);
    string name = "hillclimb_" + to_string(params.h)+"_"+to_string(params.r);
    cout << "name : " << name << endl;

    for ( int i = 0; i <= params.range; ++i ) {
        const int s = params.st_s+i;
        cout << "\rs = " << s << endl;

        for ( int j = 0; j < params.exe; ++j ) {
            cout << "\rj = " << j+1 << flush;

            Graph::set_seed(seed_gen());
            Graph graph = Graph::make(s, params.h, params.r, 1);
            Graph best = hillclimb(graph, params.alpha, seed_gen(), select_edges_noraml);
            results[i].add_result(best);
        } 
        cout << "\r";
    }

    string tmp = path+"/"+name;
    output(tmp, params, results);
}

void output(string name, ClimbParams& params, vector<Results>& results ) {
    string fname = name+".dat";

    ofstream ofs(fname);
    if ( !ofs ) {
        cout << "[" << fname << "]が開けませんでした" << endl;
        cin.get();
        exit(1); 
    }

    char buffs[256];

    ofs << "Type Name: " << name << endl;

    sprintf(buffs, "start_s %d range %d h %d r %d", params.st_s, params.range, params.h, params.r);
    ofs << buffs << endl;

    sprintf(buffs, "seed %d kick_limit %d exe %d alpha %.2lf", params.seed, params.limt, params.exe, params.alpha);
    ofs << buffs << endl;

    for ( int i = 0; i <= params.range; ++i ) {
        int s = params.st_s+i;
        Results res = results[i];

        ofs << endl;
        ofs << "[Result] s=" << s << endl;
        sprintf(buffs, "avg_diam %.5lf avg_haspl %.5lf", res.get_avg_diam(), res.get_avg_haspl());
        ofs << buffs << endl;

        sprintf(buffs, "best %d %.5lf", res.get_best().first, res.get_best().second);
        ofs << buffs << endl;
    }

    cout << "Output to " << fname << endl;
}


int main()
{
    debug_off();
    annealing_log_off_all();

    int st_s = 25;
    int range = 15;
    int h = 32, r = 4;
    int limt = 10, exe = 3;
    int seed = 31433;
    double alpha = 0.6;

    ClimbParams params( st_s, range, h, r, limt, exe, seed, alpha);
    exp_hillclimb("h32_r4", params);

}