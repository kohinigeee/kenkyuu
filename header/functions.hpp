#ifndef INCLUDE_FUNCTIONS_HPP
#define INCLUDE_FUNCTIONS_HPP

#include<vector>

template <typename T> 
void swap( T x, T y ) {
    T tmp;
    tmp = x;
    x = y;
    y = tmp;
}

template <typename T>
bool operator==(const std::vector<T>& v1, const std::vector<T>& v2) {
    if ( v1.size() != v2.size() ) return false;
    for ( int i = 0; i < v1.size() ) {
        if ( v1[i] != v2[i] ) return false;
    }
    return true;
}

#endif