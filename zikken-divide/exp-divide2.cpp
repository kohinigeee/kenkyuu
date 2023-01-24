#include<iostream>
#include<time.h>

#include "../header/Header_include.hpp"
using namespace std;

void output(string dir, int s, int h, int r, vector<pair<int,double>>& results) {

    string fname = "s"+to_string(s)+"_h"+to_string(h)+"_r"+to_string(r)+".txt";
    string path = dir+"/"+fname; 
    ofstream ofs(path);
    if ( !ofs ) {
        cout << "[" << path << "]が開けませんでした" << endl;
        cin.get();
        exit(1); 
    }
    char buffs[256];

    sprintf(buffs, "s %d h %d r %d\n", s, h, r);
    ofs << buffs;

    for ( auto p : results) {
        sprintf(buffs, "%d %.5lf\n", p.first, p.second);
        ofs << buffs;
    }

    ofs.close();
    cout << "Output to " << path << endl;
}
//整数計画利
void func_divide1(int s, int h, int r, const int seed, int exe) {
    mt19937 mt;
    mt.seed(seed);
    vector<pair<int,double>> results;
    for ( int i = 40; i <= 90; i += 5 ) {
        double t = double(i)/100;
        double sum = 0;
        for ( int j = 0; j < exe; ++j ) {
        cout << "t = " << t << ", exe = " << j << endl;
        divide_beta = t;
        Graph best = divideHillclimb_1(s, h, r, mt());
        pair<int, double> hp = best.calc_haspl();
        cout << hp.second << endl;
        sum += hp.second;
        }
        results.push_back(make_pair(i, sum/exe));
    }
    output("results2", s, h, r, results);
}

Graph divideinit(int s, int h, int r, int seed ) {
    mt19937 mt;
    mt.seed(seed);

    Graph::set_seed(mt());
    Graph graph = divideHillclimb_1(s, h, r, mt());

    Graph best2 = hillclimb(graph, 0.5, mt(), select_edges_noraml, 0, defo_history);
    return best2;
}

Graph hill(int s, int h, int r, int seed) {
    mt19937 mt;
    mt.seed(seed);

    Graph::set_seed(mt());
    Graph graph = Graph::make(s, h, r, 1);
    return hillclimb(graph, 0.5, mt(), select_edges_noraml, 0, defo_history);
}

int main()
{
    annealing_log_off_all();
    debug_off();

    mt19937 mt;
    mt.seed(848);

    int s = 90, h = 432, r = 12;

    // func_divide1(42, 80, 6, mt(), 5);
    // func_divide1(35, 32, 4, mt(), 5);
    // func_divide1(90, 432, 12, mt(), 5);
    func_divide1(350, 1024, 10, mt(), 5);
    // func_divide1(1100, 1024, 5, mt(), 5);
    // func_divide1(105, 1281, 21, mt(), 5);
    // func_divide1(300, 3800, 30, mt(), 5);
}