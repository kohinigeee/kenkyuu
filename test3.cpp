#include<iostream>
#include<vector>

#include"header/Header_include.hpp"

using namespace std;

int main()
{
    debug_off();
    annealing_log_off_all();

    const int s = 30;
    const int h = 32;
    const int r = 4;


    try {
        Graph::set_seed(5);
        Graph graph = Graph::make(s, h, r, 1);
        Params param = Params();

        Graph best = divideAnnealing(param, s, h, r, 0); 


        best.toDot("graph3.dot", best, 5.0);
        best.print();
        
    } catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}

