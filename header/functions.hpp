#ifndef INCLUDE_FUNCTIONS_HPP
#define INCLUDE_FUNCTIONS_HPP

#include<vector>

template <typename T>
bool operator==(const std::vector<T>& v1, const std::vector<T>& v2) {
    if ( int(v1.size()) != int(v2.size()) ) return false;
    for ( int i = 0; i < v1.size(); ++i ) {
        if ( !(v1[i] == v2[i]) ) return false;
    }
    return true;
}

#endif