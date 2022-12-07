#include<iostream>
#include<vector>
#include<string>

#include"./header/Header_include.hpp"

using namespace std;

int main() {
    debug_off();
    annealing_log_off_all();

    const int s = 25;
    const int h = 32, r = 4;
    const int free = 0;

    Params param = Params();

    try {
        Graph graph = divideAnnealing(param, s-free, h, r, free);

        graph.linkLoops(true);
        graph.toDot("graph2.dot", graph, 5.0);
        // graph.print();

        cout << endl;
        cout << "finished annealing" << endl;
        if ( graph.isPropGraph() ) cout << "correct graph" << endl;
        else cout << "not correct graph" << endl;

    } catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}