#include<iostream>
#include<time.h>

#include "../header/Header_include.hpp"

using namespace std;

void output(string path, string name, GraphInfo& info, double dura); 

void func_hill(int s, int h, int r, string path, const int seed ) {
    time_t st_time = clock();
    Graph graph = Graph::make(s, h, r, 1);
    GraphInfo tmp = graph.calcInfo();
    output(path, "initil graph", tmp, -1);
    Graph best = hillclimb(graph, 0.3, seed, select_edges_hl, 0);
    time_t fin_time = clock();

    double dura = double(fin_time-st_time)/CLOCKS_PER_SEC;
    GraphInfo info = best.calcInfo();

    output(path, "hillclimb", info, dura);
}

//整数計画利
void func_divide1(int s, int h, int r, string path, const int seed ) {
    time_t st_time = clock();
    Graph best = divideHillclimb_1(s, h, r, seed);
    time_t fin_time = clock();

    double dura = double(fin_time-st_time)/CLOCKS_PER_SEC;
    GraphInfo info = best.calcInfo();

    output(path, "divide with problem", info, dura);
}

void func_divide2(int s, int h, int r, string path, const int seed ) {
    time_t st_time = clock();
    Graph best = divideHillclimb_2(s, h, r, seed);
    time_t fin_time = clock();

    double dura = double(fin_time-st_time)/CLOCKS_PER_SEC;
    GraphInfo info = best.calcInfo();

    output(path, "divide normal", info, dura);
}

void output(string path, string name, GraphInfo& info, double dura) {

    ofstream ofs(path, std::ios::app);
    if ( !ofs ) {
        cout << "[" << path << "]が開けませんでした" << endl;
        cin.get();
        exit(1); 
    }
    char buffs[256];

    ofs << "Type Name: " << name << endl;

    sprintf(buffs, "diam: %d, haspl: %.5lf, time: %.2lf", info.get_diam(), info.get_haspl(), dura);
    ofs << buffs << endl;
    ofs.close();

    cout << "Output to " << path << endl;
}

void func( string path,  int s, int h, int r, mt19937& mt) {
    ofstream ofs(path, ios::app);
    char buff[256];

    sprintf(buff, "s: %d, h: %d, r:%d\n", s, h, r);
    ofs << buff;
    ofs.close();
   
   cout << "Start function with s=" << s << ", h=" << h << ", r=" << r << endl;
   cout << "start func hill" << endl;
   func_hill(s, h, r, path, mt()); 
   cout << "end func hill" << endl;

   cout << "start divide1" << endl;
   func_divide1(s, h, r, path, mt());
   cout << "end divide1" << endl;

   cout << "start divide2" << endl;
   func_divide2(s, h, r, path, mt());
   cout << "end divide2" << endl;

    ofs = ofstream(path, ios::app);
   ofs << endl;
   ofs.close();
   cout << "finished function\n" << endl;
}

int main()
{
    annealing_log_off_all();
    debug_off();

    const int s = 40;
    const int h = 32;
    const int r = 4;
    int seed = 50;

    long long limt_time = 60*30;

    set_hill_limt_time(limt_time);

    string path = "results/result.txt";

    mt19937 mt;
    mt.seed(seed);
    // func(path, 40, 32, 4, mt);
    // func(path, 40, 80, 6, mt);
    // func(path, 12, 128, 24, mt);
    // func(path, 90, 432, 12, mt);
    // func(path, 110, 1281, 21, mt);
    // func(path, 1280, 1024, 5, mt);
    // func(path, 350, 1024, 10, mt);
    // func(path, 330, 3800, 30, mt);
    func(path, 280, 4608, 36, mt);
    func(path, 440, 8208, 48, mt);
    // func(path, 5100, 10000, 10, mt);
}