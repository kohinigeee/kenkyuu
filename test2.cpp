#include<iostream>
#include<random>
#include<algorithm>
#include<string>
#include<cstdio>

#include"header/Header_include.hpp"

using namespace std;


int main(int argc, char** args ) 
{
    debug_off();
    annealing_log_off_all();
    annealing_log_on(0);
    
    const int s = 46, h = 80, r = 6;


    try {
        Graph graph = Graph::make(s, h, r, 1);
        Graph best = hillClimbing_mdstkick(graph, 20000, 10, select_edges_noraml);

        graph_info_t best_info = best.sumD();
        double haspl = best.calcHaspl(best_info["sumd"]);

        cout << endl;
        cout << "diam : " << best_info["diam"] << endl;
        cout << "haspl: " << haspl << endl;
    } catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}