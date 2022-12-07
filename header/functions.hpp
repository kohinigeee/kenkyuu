#ifndef INCLUDE_FUNCTIONS_HPP
#define INCLUDE_FUNCTIONS_HPP

#include<vector>
#include<utility>
#include<random>

template <typename T>
bool operator==(const std::vector<T>& v1, const std::vector<T>& v2) {
    if ( int(v1.size()) != int(v2.size()) ) return false;
    for ( int i = 0; i < v1.size(); ++i ) {
        if ( !(v1[i] == v2[i]) ) return false;
    }
    return true;
}

template <typename T>
bool operator==(const std::pair<T, T>& p1, const std::pair<T, T>& p2) {
    return (p1.first == p2.first && p1.second == p2.second);
}

void stochars(const string& s, char* chs ) {
    for ( int i = 0; i < s.size(); ++i ) {
        chs[i] = s[i];
    }
    chs[s.size()] = '\0';
}

vector<int> makeRandomTree(int s, int r, mt19937& mt) {
    vector<int> p(s, -1);
    vector<int> cnt(s, 0);
    vector<int> pool; //頂点を接続できる木上の要素
    vector<int> remain(s, 0); //まだ木に接続されていない頂点

    for ( int i = 0; i < s; ++i ) remain[i] = i;

    uniform_int_distribution<int> div(0, s-1);
    int root = div(mt);

    pool.push_back(root);
    swap(remain[root], remain.back());
    remain.pop_back();

    while(remain.size() > 0 ) {
        uniform_int_distribution<int> div2(0, remain.size()-1);
        int idx = div2(mt);
        int no = remain[idx];
        swap(remain[idx], remain.back());
        remain.pop_back();

        uniform_int_distribution<int> div_pool(0, pool.size()-1);
        int idx_pool = div_pool(mt);
        int p_no = pool[idx_pool];

        p[no] = p_no;
        cnt[no] += 1;
        cnt[p_no] += 1;
        if ( cnt[p_no] >= r ) {
            swap(pool[idx_pool], pool.back());
            pool.pop_back();
        }
        pool.push_back(no);
    }

    return p;
}
#endif