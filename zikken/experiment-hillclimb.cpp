#include<iostream>
#include<vector>
#include<cstdio>

#include "../header/Header_include.hpp"

using namespace std;


int main()
{
    annealing_log_off_all();
    debug_off();

    int const range = 10;
    int const n = 20000;
    int const N = 5;

    const int s = 28;
    const int h = 32;
    const int r = 4;
    const int seed = 324;

    mt19937 seed_gen;
    seed_gen.seed(seed);

    for ( int i = 0; i <= range; ++i ) {
        const int tmps = s+i;

        for ( int j = 0; j < N; ++j ) {
            Graph::set_seed(seed_gen());
            Graph graph = Graph::make(s, h, r, 1);
            // Graph graph = Graph::makeRandom(s, h, r);
            Graph best = hillClimbing_mdstkick(graph, n, seed_gen(), select_edges_noraml);

            graph_info_t info = best.sumD();
            double haspl = best.calcHaspl(info["sumd"]);

            cout << endl;
            printf("s = %d, h = %d, r = %d\n", tmps, h, r);
            printf("diam = %d, haspl = %lf\n", info["diam"], haspl);
        }
    }
}