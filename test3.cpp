#include<iostream>
#include<vector>

#include"header/Header_include.hpp"

using namespace std;

int main()
{
    debug_off();
    annealing_log_off_all();

    const int s = 20;
    const int h = 32;
    const int r = 4;


    try {
        Graph::set_seed(5);
        Graph graph = Graph::make(s, h, r, 1);
        Params param = Params();

        Graph best = annealing(graph, param, select_edges_noraml);
        Graph::toDot("graph1.dot", best, 4);

        Graph mds = makeMDTgraph(best);
        Graph::toDot("graph2.dot", mds, 4);
        graph_info_t info_mds = mds.sumD();
        double haspl_mds = mds.calcHaspl(info_mds["sumd"]);
        
        mds.linkLoops();
        Graph::toDot("graph3.dot", mds, 4);
        cout << "islinking = " << mds.isLinking() << endl;

        graph_info_t info = mds.sumD();
        double haspl = mds.calcHaspl(info["sumd"]);
        cout << "finished" << endl;
        
        cout << "diam(mds) = " << info_mds["diam"] << endl;
        cout << "haspl(mds) = " << haspl_mds << endl;
        cout << "diam = " << info["diam"] << endl;
        cout << "haspl = " << haspl << endl;

        
    } catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}

