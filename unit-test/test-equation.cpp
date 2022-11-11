#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<cmath>
#include<cstdio>

#include"../header/Edge.hpp"
#include"../header/Nodes.hpp"
#include"../header/GraphPart.hpp"
#include"../header/Graph.hpp"

#define INF (1LL<<60)

using namespace std;

int main()
{
    G_no g_no1(1), g_no2(2);
    G_no g_no3 = g_no1;

    if ( ( g_no1 == g_no2 ) == false ) cout << "Ok";
    else cout << "No" << endl;
}