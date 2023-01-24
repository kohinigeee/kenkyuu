#include<iostream>
#include<random>
#include<algorithm>
#include<string>
#include<cstdio>

#include"header/Header_include.hpp"

using namespace std;
using pid = pair<int, long long>;


int main(int argc, char** args ) 
{
    debug_off();
    annealing_log_off_all();
    annealing_log_on(0);
    
    // const int s = 90, h = 432, r = 12;
    int s = 400, h = 8208, r = 48;
    int seed = 484;
    mt19937 mt;
    mt.seed(seed);

    double sum1 = 0, sum2 = 0;

    int exe = 100;
    try {

        for ( int i = 0; i < exe; ++i ) {
            cout << "i = " << i << endl;
            Graph::set_seed(mt());
            Graph graph = Graph::make(s, h, r, 1);

            time_t st = clock();
            pid p1 = graph.calc_dis();
            time_t end = clock();
            sum1 += (double)(end-st);
            
            st = clock();
            pid p2 = graph.calc_dis3();
            end = clock();
            sum2 += (double)(end-st);

            // cout << "p1 = " << p1.first << ", " << p1.second << endl;
            // cout << "p2 = " << p2.first << ", " << p2.second << endl;
        }
        cout << "average time1 = " << sum1/exe << endl;
        cout << "average time2 = " << sum2/exe << endl;

    } catch ( IregalManuplateException e ) {
        cout << e.getMesage() << endl;
    } catch ( IregalValueException e ) {
        cout << e.getMesage() << endl;
    }
}