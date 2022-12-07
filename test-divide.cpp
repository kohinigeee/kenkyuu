#include<iostream>
#include<vector>
#include<string>
#include<ctime>

#include"./header/Header_include.hpp"

using namespace std;
using ll = long long;

int main() {
    debug_off();
    annealing_log_off_all();

    const int s = 1400;
    const int h = 10000, r = 10;
    const int free = 0;
    const int n = 10;
    Params param = Params();

    mt19937 seed_gen;
    seed_gen.seed(param.get("seed"));


    Results result_div("divited_annealing");
    Results result("normal_annealing");
    long long sumtime_div = 0, sumtime = 0;

    try {
        for ( int i = 0; i < n; ++i ) {
            cout << "\ri = " << i << flush;
            Params tmp_param = param;
            param.set("seed", seed_gen());
            Graph::set_seed(seed_gen());

            clock_t t_start, t_end;

            t_start = clock();
            Graph graph = Graph::make(s, h, r, 1);
            Graph best = annealing(graph, tmp_param, select_edges_noraml);
            t_end = clock();
            sumtime += ll(t_end-t_start);
            result.add_result(best);

            t_start = clock();
            Graph best_div = divideAnnealing(tmp_param, s, h, r );
            t_end = clock();
            sumtime_div += ll(t_end-t_start);
            result_div.add_result(best_div);
        }

        cout << "\n Finished" << endl;

        cout << "[Result: normal_anneaing]" << endl;
        result.print();
        cout << "avg_time: " << sumtime/double(n) << endl;
        cout << endl;

        cout << "[Result: divide_annealing]" << endl;
        result_div.print();
        cout << "avg_time: " << sumtime_div/double(n) << endl;
        cout << endl;
    } catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}