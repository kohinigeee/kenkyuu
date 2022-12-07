#ifndef INCLUDE_RESULT_HPP
#define INCLUDE_RESULT_HPP

#include<iostream>
#include<string>
#include<algorithm>
#include<cstdio>

#include"Graph.hpp"
#define RESULT_INF (1LL<<60)

using namespace std;

class Results {
    int cnt;
    double sum_haspl, sum_diam;
    double min_haspl;
    long long min_diam; 
    string name;

    public:
    Results(string name) : name(name) {
        cnt = 0;
        sum_haspl = sum_diam = 0;
        min_haspl = double(RESULT_INF);
        min_diam =  RESULT_INF;
    }

    void add_result(Graph& graph) {
        graph_info_t info = graph.sumD();
        double haspl =  graph.calcHaspl(info["sumd"]);
        long long diam = info["diam"];

        sum_haspl += haspl;
        sum_diam += diam;
        min_haspl = min(min_haspl, haspl );
        min_diam = min(min_diam, diam);
        ++cnt;
    } 

    double get_avg_haspl() { return sum_haspl/cnt; }
    double get_avg_diam() { return sum_diam/double(cnt); }
    double get_min_haspl() { return min_haspl; }
    double get_min_diam() { return min_diam; }

    void print();    
};

void Results::print() {
    cout << name << endl;
    printf("avg_haspl= %.5lf  avg_diam= %.5lf\n", get_avg_haspl(), get_avg_diam());
    printf("min_haspl= %.5lf  min_diam= %lf\n", get_min_haspl(), get_min_diam());
}

#endif