#include<iostream>
#include"../header/Header_include.hpp"

using namespace std;


int main()
{
    annealing_log_off_all();
    debug_off();

    const int s = 32, h = 32, r = 4;
    try {
        Params param = Params();

        Graph best = divideAnnealing(param, s, h, r);

        graph_info_t info = best.sumD();
        double haspl = best.calcHaspl(info["sumd"]);

        cout << "diam : " << info["diam"]  << endl;
        cout << "haspl : " << haspl << endl;
    } catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}