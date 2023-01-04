#include<iostream>
#include<iostream>
#include<vector>

#include"../header/Header_include.hpp"

using namespace std;

int main()
{
    debug_off();
    annealing_log_off_all();

    vector<Results> results;

    int limcnt = 10;
    int seed = 10;
    int st_s = 25;
    int range = 15;
    const int h = 32;
    const int r = 4;

    const int exe = 3;

    mt19937 seed_gen;
    seed_gen.seed(seed);
    results = vector<Results>(range+1);

    try {
        for ( int i = 0; i <= range; ++i ) { 
            const int s = i+st_s;
            cout << "s = " << s << endl;

            for ( int j = 0; j < exe; ++ j ) {
            cout << "\rj = " << j+1; cout << flush;
            Graph::set_seed(seed_gen());
            mt19937 mt; mt.seed(seed_gen());

            Graph graph = Graph::make(s, h, r, 1);
            // Graph best = directHillclimbWithKick(graph, mt, limcnt, directHillclimb_once_2);
            Graph best = hillclimb(graph, 0.5, seed_gen(), select_edges_noraml);
            results[i].add_result(best);
            }
        }

        cout << "[Results]" << endl;
        for ( int i = 0; i <= range; ++i ) {
            int s = i+st_s;
            cout << "s = " << s << endl;
            results[i].print();
            cout << endl;
        }
    } catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}