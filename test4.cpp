
#include<iostream>
#include<vector>
#include<queue>

using namespace std;

void printvec(vector<vector<int>> & A ) {
    for ( auto tmp : A ) {
        for ( auto v : tmp ) cout << v << " "; cout << endl;
    }
}

void func(vector<vector<int>> nbr) {
    int level = 1;
    long long dis = 0;
    int diam = 2;
    long long cnt = 0;
    int s = nbr.size();

    vector<vector<int>> A ( s, vector<int>(s, 0));
    vector<vector<int>> B ( s, vector<int>(s, 0));

    for ( int i = 0; i < s; ++i ) A[i][i] = B[i][i] = 1;

    while(true) {
        for ( int i = 0; i < s; ++i ) {
            for ( int n : nbr[i] ) {
                for ( int j = 0; j < s; ++j ) {
                    ++cnt;
                    if ( B[i][j] == 1 || A[n][j] == 1 ) B[i][j] = 1;
                }
            }
        }

        
        int num = 0;
        for ( int i = 0; i < s; ++i ) {
            for ( int j = 0; j < s; ++j ) {
                num += B[i][j];
            }
        }
        if ( num == s*s ) break;
        printvec(A);
        swap(A,B);
        ++level;
        cout << "cnt = " << cnt << endl;
        cout << "dis = " << dis << endl;
    }
}


int main()
{
    vector<vector<int>> vec;
    vec.push_back({2, 3});
    vec.push_back({5});
    vec.push_back({0, 3,4});
    vec.push_back({0, 2, 9, 5});
    vec.push_back({2, 7, 9});
    vec.push_back({1, 3});
    vec.push_back({9});
    vec.push_back({4,8});
    vec.push_back({7});
    vec.push_back({3,4,6});


    func(vec);
}
