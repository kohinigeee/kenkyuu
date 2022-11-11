#ifndef INCLUDE_DEBUG_HTTP
#define INCLUDE_DEBUG_HTTP

#include<string>
#include<iostream>

#define DEB() if ( is_deb ) 

bool is_deb = true;

void debug_on() {
    is_deb = true;
}

void debug_off() {
    is_deb = false;
}

template <typename T>
void deb_eq(T& t1, T& t2, std::string name = "Type" ) {
    if ( t1 == t2 ) std::cout << "Same " << name << std::endl;
    else std::cout << "Don't Same" << name << std::endl;
}

#endif