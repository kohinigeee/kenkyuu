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
    pair<long long, double> best_result;
    string name;

    public:
    Results() : Results("No name") {}
    Results(string name) : name(name) {
        cnt = 0;
        sum_haspl = sum_diam = 0;
        min_haspl = double(RESULT_INF);
        min_diam =  RESULT_INF;
        best_result = make_pair(RESULT_INF, double(RESULT_INF));
    }

    void add_result(Graph& graph) {
        GraphInfo info = graph.calcInfo();
        long long diam = info.get_diam();
        double haspl = info.get_haspl();
        pair<long long, double> result = make_pair(diam, haspl);

        sum_haspl += haspl;
        sum_diam += diam;
        min_haspl = min(min_haspl, haspl );
        min_diam = min(min_diam, diam);
        best_result = min(best_result, result);

        ++cnt;
    } 

    double get_avg_haspl() { return sum_haspl/cnt; }
    double get_avg_diam() { return sum_diam/double(cnt); }
    double get_min_haspl() { return min_haspl; }
    double get_min_diam() { return min_diam; }
    pair<long long, double> get_best() { return best_result; }

    void print();    
};

void Results::print() {
    cout << name << endl;
    printf("avg_haspl= %.5lf  avg_diam= %.5lf\n", get_avg_haspl(), get_avg_diam());
    printf("min_haspl= %.5lf  min_diam= %lf\n", get_min_haspl(), get_min_diam());
    printf("best_result: diam = %ld, haspl = %.5lf\n", best_result.first, best_result.second);
}

#endif