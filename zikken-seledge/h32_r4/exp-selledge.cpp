#include<iostream>
#include<sys/stat.h>

#include"../../header/Header_include.hpp"
#include"../../header/functions.hpp"

using namespace std;

void output(string path, string name, int s, int h, int r, History& his) {
    string fname = path+"/"+name+".dat";
    mkdir(path.c_str());

    ofstream ofs(fname);
    if ( !ofs ) {
        cout << fname << " can't open" << endl;
        exit(1);
    }

    char buff[256];
    ofs << "type : " << name << endl;
    sprintf(buff, "s %d h %d r %d\n", s, h, r);
    ofs << buff;
    ofs << "counts " << his.values.size() << endl;

    for ( auto p : his.values ) {
        sprintf(buff, "%d %lf\n", p.first, p.second);
        ofs <<  buff;
    }
    ofs.close();
}

History exp_hill(int s, int h, int r, int seed, Graph graph) {
    History his(ON_HISTORY);
    GraphInfo info = graph.calcInfo();

    his.add(make_pair(info.get_diam(), info.get_haspl()));

    hillclimb(graph, 0.3, seed, select_edges_hl, 0, his);
    return his;
}

void do_hill(int s, int h, int r, int seed, Graph graph) {
    char buff[256];
    
    sprintf(buff, "s%d_h%d_r%d", s, h, r);
    string path = string(buff);


    History his = exp_hill(s, h, r, seed, graph);
    string name = "hillclimb";
    output(path, name, s, h, r, his);
}

History exp_dir2(int s, int h, int r, Graph graph) {
    History his(ON_HISTORY);

    GraphInfo info = graph.calcInfo();

    his.add(make_pair(info.get_diam(), info.get_haspl()));
    directHillclimb_once_2(graph, 0, -1, his);
    return his;
}

void do_dir2(int s, int h, int r, Graph graph) {
    char buff[256];

    sprintf(buff, "s%d_h%d_r%d", s, h, r);
    string path = string(buff);

    History his = exp_dir2(s, h, r, graph);
    string name = "directed2";
    output(path, name, s, h, r, his);
}

History exp_dir1(int s, int h, int r, Graph graph) {
    History his(ON_HISTORY);

    GraphInfo info = graph.calcInfo();

    his.add(make_pair(info.get_diam(), info.get_haspl()));
    directHillclimb_once(graph, 0, -1, his);
    return his;
}

void do_dir1(int s, int h, int r, Graph graph) {
    char buff[256];

    sprintf(buff, "s%d_h%d_r%d", s, h, r);
    string path = string(buff);

    History his = exp_dir1(s, h, r, graph);
    string name = "directed1";
    output(path, name, s, h, r, his);
}

void func(int s, int h, int r, mt19937& mt) {
    Graph::set_seed(mt());

    Graph graph = Graph::make(s, h, r, 1);

    do_hill(s, h, r, mt(), graph);
    do_dir1(s, h, r, graph);
    do_dir2(s, h, r, graph);

}
int main()
{
    annealing_log_off_all();
    debug_off();

    int s = 33;
    int h = 32;
    int r = 4;
    int seed = 324;


    mt19937 seed_gen;
    seed_gen.seed(seed);

    for ( int i = 0; i <= 6; ++i ) {
        int tmps = 3*i+s;
        cout << "s = " << tmps << ", h = " << h << ", r = " << r << endl;
        func(tmps, h, r, seed_gen);
    }
    cout << "finished" << endl;
}