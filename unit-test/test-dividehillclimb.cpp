#include<iostream>
#include"../header/Header_include.hpp"

using namespace std;


int main()
{
    annealing_log_off_all();
    debug_off();

    const int s = 32, h = 32, r = 4;
    try {

        divideInfo dinfo = divideInfo::makeDivide_ports(s, h, r);
        // Graph best = divideHillClimb(dinfo, 5000, 1, select_edges_noraml, dividehillClimbing_normalkick); 
        Graph best = divideHillClimb( dinfo, 5000, 1, select_edges_noraml, dividehillClimbing_mdstkick);
        best.linkLoops();

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