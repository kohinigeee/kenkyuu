#include<iostream>
#include<vector>
#include<queue>

using namespace std;

void printvec(vector<vector<int>> & A ) {
    for ( auto tmp : A ) {
        for ( auto v : tmp ) cout << v << " "; cout << endl;
    }
}

void func(vector<vector<int>>& nbr) {
    int level = 1;
    long long dis = 0;
    int diam = 2;
    long long cnt = 0;

    int s = nbr.size();
    vector<int> hdgree;
    queue<pair<int,int>> new_bits;

    
    vector<vector<int>> A ( s, vector<int>(s, 0));
    vector<vector<int>> B ( s, vector<int>(s, 0));

    for ( int i = 0; i < s; ++i ) A[i][i] = B[i][i] = 1;

    for ( int i = 0; i < s; ++i ) new_bits.push(make_pair(i, i));

    printvec(B);
    while(true) {
        cout << "calc2: level = " << level << endl;
        int tmp_size = new_bits.size();
        for ( int _ = 0; _ < tmp_size; ++_ ) {
            pair<int,int> newbit = new_bits.front(); new_bits.pop();
            // cout << "new_bit = " << newbit.first << ", " << newbit.second << endl;
            for ( auto n : nbr[newbit.first] ) {
                ++cnt;
                if ( B[n][newbit.second] == 0 ) {
                    B[n][newbit.second] = 1;
                    new_bits.push(make_pair(n, newbit.second));
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
        A = B;
        ++level;
        cout << "dis = " << dis << endl;
        // if ( level >= 3 ) return;
    }

    cout << "calc_dis2 : cnt = " << cnt << endl;
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