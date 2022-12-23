#include<iostream>
#include"../header/Header_include.hpp"

using namespace std;


int main()
{
    annealing_log_off_all();
    debug_off();

    const int s = 32, h = 32, r = 4;
    const int seed = 10;
    try {

        Graph graph = Graph::make(s, h, r, 1);
        Graph best = hillClimbing_normalkick(graph, 5000, seed, select_edges_noraml);
        graph_info_t info = best.sumD();
        double haspl = best.calcHaspl(info["sumd"]);

        best.toDot("graph1.dot", best, 5.0);

        best.print("Best");
        cout << "diam : " << info["diam"]  << endl;
        cout << "haspl : " << haspl << endl;
    } catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}