#ifndef INCLUDE_FUNCTIONS_HPP
#define INCLUDE_FUNCTIONS_HPP

template <typename T> 
void swap( T x, T y ) {
    T tmp;
    tmp = x;
    x = y;
    y = tmp;
}

#endif