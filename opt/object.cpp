#include<iostream>

using namespace std;

double funct(double x) {
    double score = (x-2)*(x-2);

    return score;
}

int main(int argc, char** args)
{
    double x = stod(args[1]);

    double score = funct(x);

    cout << score;
}