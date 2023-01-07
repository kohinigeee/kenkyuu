#include<iostream>
#include<vector>
#include<string>
#include<thread>
#include<mutex>
#include<map>

#include"../header/Header_include.hpp"
#include"../header/DirectHillclimb.hpp"
#include"../header/functions.hpp"
#include"ClimbParams.hpp"

using namespace std;

mutex mx;

void output(string, ClimbParams&, vector<Results>&);

void exp_hillcimb(vector<Results>& results, string name, int s, ClimbParams params) {
    mt19937 seed_gen;
    seed_gen.seed(params.seed);

    char buff[256];
    char tmp[256];
    stochars(name, tmp);
    
    int idx = s-params.st_s;
    for ( int j = 0; j < params.exe; ++j ) {
        sprintf(buff, "name = %s, s=%d, j=%d\n", tmp, s, j); 
        cout << buff << flush;
        
        Graph::set_seed(seed_gen());
        Graph graph = Graph::make(s, params.h, params.r, 1);
        Graph best = hillclimb(graph, params.alpha, seed_gen(), select_edges_hl);

        mx.lock();
        results[idx].add_result(best); 
        mx.unlock();
    }
}

void do_hillclimb(string path, ClimbParams params ) {
    mt19937 seed_gen;
    seed_gen.seed(params.seed);
 
    vector<Results> results(params.range+1);
    map<int, thread> thmap;

    string name = "hillclimb_" + to_string(params.h)+"_"+to_string(params.r);
    char buffs[256];
    char name_ch[256];

    for ( int i = 0; i <= params.range; ++i ) {
        const int s = params.st_s+i;
        ClimbParams tmp  = params;
        params.seed = seed_gen();

        thmap[s] = thread(exp_hillcimb, ref(results), name, s, tmp);
    }

    for ( auto& p : thmap ) p.second.join();

    string tmp = path+"/"+name;
    output(tmp, params, results);
}


void exp_direc1(vector<Results>& results, string name, int s, ClimbParams params) {
    mt19937 seed_gen;
    seed_gen.seed(params.seed);

    char buff[256];
    char tmp[256];
    stochars(name, tmp);
    
    int idx = s-params.st_s;
    for ( int j = 0; j < params.exe; ++j ) {
        sprintf(buff, "name = %s, s=%d, j=%d\n", tmp, s, j); 
        cout << buff << flush;
        
        Graph::set_seed(seed_gen());
        Graph graph = Graph::make(s, params.h, params.r, 1);
        directHillclimb_once(graph);

        mx.lock();
        results[idx].add_result(graph); 
        mx.unlock();
    }
}

void do_direct1(string path, ClimbParams params ) {
    mt19937 seed_gen;
    seed_gen.seed(params.seed);
 
    vector<Results> results(params.range+1);
    map<int, thread> thmap;

    string name = "direct1_" + to_string(params.h)+"_"+to_string(params.r);
    char buffs[256];
    char name_ch[256];

    for ( int i = 0; i <= params.range; ++i ) {
        const int s = params.st_s+i;
        ClimbParams tmp  = params;
        params.seed = seed_gen();

        thmap[s] = thread(exp_direc1, ref(results), name, s, tmp);
    }

    for ( auto& p : thmap ) p.second.join();

    string tmp = path+"/"+name;
    output(tmp, params, results);
}

void exp_directhillclimb(string path, ClimbParams& params ) {
    mt19937 seed_gen;
    seed_gen.seed(params.seed);
 
    vector<Results> results(params.range+1);
    string name = "directHillclimb_" + to_string(params.h)+"_"+to_string(params.r);
    char buffs[256];
    char name_ch[256];

    stochars(name, name_ch); 
    // cout << "name : " << name << endl;
    long long sum = params.exe*(params.range+1);
    long long cnt = 0;

    for ( int i = 0; i <= params.range; ++i ) {
        const int s = params.st_s+i;

        for ( int j = 0; j < params.exe; ++j ) {
            double prop = cnt/(double)sum;
            sprintf(buffs, "name = %s, s=%d, j=%d  [%.1lf%]\n", name_ch, s, j, prop*100);
            cout << buffs << flush; 

            Graph::set_seed(seed_gen());
            mt19937 mt; mt.seed(seed_gen());
            Graph graph = Graph::make(s, params.h, params.r, 1);
            Graph best = directHillclimbWithKick(graph, mt, params.limt, directHillclimb_once_2);
            results[i].add_result(best);

            ++cnt;
        }
    }

    string tmp = path+"/"+name;
    output(tmp, params, results);
}


void exp_direct_1(string path, ClimbParams& params ) {
    mt19937 seed_gen;
    seed_gen.seed(params.seed);
 
    vector<Results> results(params.range+1);
    string name = "direct1_" + to_string(params.h)+"_"+to_string(params.r);
    char buffs[256];
    char name_ch[256];

    stochars(name, name_ch); 
    // cout << "name : " << name << endl;
    long long sum = params.exe*(params.range+1);
    long long cnt = 0;

    for ( int i = 0; i <= params.range; ++i ) {
        const int s = params.st_s+i;

        for ( int j = 0; j < params.exe; ++j ) {
            double prop = cnt/(double)sum;
            sprintf(buffs, "name = %s, s=%d, j=%d  [%.1lf%]\n", name_ch, s, j, prop*100);
            cout << buffs << flush; 

            Graph::set_seed(seed_gen());
            mt19937 mt; mt.seed(seed_gen());
            Graph graph = Graph::make(s, params.h, params.r, 1);
            // Graph best = directHillclimb(graph, mt, params.alpha, directHillclimb_once);
            directHillclimb_once(graph);
            Graph best = graph;
            results[i].add_result(best);

            ++cnt;
        } 
    }

    string tmp = path+"/"+name;
    output(tmp, params, results);
}

//GraphTree利用
void exp_direct_2(string path, ClimbParams& params ) {
    mt19937 seed_gen;
    seed_gen.seed(params.seed);
 
    vector<Results> results(params.range+1);
    string name = "direct2_" + to_string(params.h)+"_"+to_string(params.r);
    char buffs[256];
    char name_ch[256];

    stochars(name, name_ch); 
    // cout << "name : " << name << endl;
    long long sum = params.exe*(params.range+1);
    long long cnt = 0;

    for ( int i = 0; i <= params.range; ++i ) {
        const int s = params.st_s+i;

        for ( int j = 0; j < params.exe; ++j ) {
            double prop = cnt/(double)sum;
            sprintf(buffs, "name = %s, s=%d, j=%d  [%.1lf%]\n", name_ch, s, j, prop*100);
            cout << buffs << flush; 

            Graph::set_seed(seed_gen());
            mt19937 mt; mt.seed(seed_gen());
            Graph graph = Graph::make(s, params.h, params.r, 1);
            // Graph best = directHillclimb(graph, mt, params.alpha, directHillclimb_once_2);
            directHillclimb_once_2(graph);
            Graph best = graph;
            results[i].add_result(best);

            ++cnt;
        } 
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

// void exp_func(string path, ClimbParams params) {
//     exp_hillclimb(path, params);
//     exp_direct_1(path, params);
//     exp_direct_2(path, params);
//     exp_directhillclimb(path, params);
// }

void func1() {
    cout << "exe Func1" << endl;
}

int main()
{
    debug_off();
    annealing_log_off_all();

    const int seed = 848;
    const int limt = 10;
    const int exe = 1;
    double alpha = 0.6;

    mt19937 mt;
    mt.seed(seed);

    vector<string> paths;
    paths.push_back("h32_r4");
    paths.push_back("h80_r6");
    paths.push_back("h128_r24");
    paths.push_back("h432_r12");
    paths.push_back("h1281_r21");

    vector<ClimbParams> params;
    params.push_back(ClimbParams(25, 25, 32, 4, limt, exe, mt(), alpha));
    params.push_back(ClimbParams(35, 25, 80, 6, limt, exe, mt(), alpha));
    params.push_back(ClimbParams(7, 10, 128, 24, limt, exe, mt(), alpha));
    params.push_back(ClimbParams(50, 30, 432, 12, limt, exe, mt(), alpha));
    params.push_back(ClimbParams(75, 35, 1281, 21, limt, exe, mt(), alpha));

    thread th1(do_hillclimb, paths[0], params[0]);
    thread th2(do_direct1, paths[0], params[0]);

    th1.join();
    th2.join();
    // thread th1(exp_func, paths[0], params[0]);
    // thread th2(exp_func, paths[1], params[1]);

    // thread th1(exp_hillclimb, paths[0], params[0]);
    // thread th2(exp_hillclimb, paths[1], params[1]);
    // thread th3(exp_hillclimb, paths[2], params[2]);
    // thread th4(exp_hillclimb, paths[3], params[3]);
    // thread th5(exp_hillclimb, paths[4], params[4]);

    // th1.join();
    // th2.join();
    // th3.join();
    // th4.join();
    // th5.join();

    return 0;
}