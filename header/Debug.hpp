#ifndef INCLUDE_DEBUG_HTTP
#define INCLUDE_DEBUG_HTTP

#include<string>
#include<iostream>

#define DEB(i) if ( deb_level >= i ) 
#define DEB_HIGH 2 
#define DEB_MIDLE 5
#define DEB_LOW 10
#define DEB_TMP 1

int deb_level = DEB_MIDLE;

void debug_on(int x) {
    deb_level = x;
}

void debug_off() {
    deb_level = 0;
}

template <typename T>
void deb_eq(T& t1, T& t2, std::string name = "Type" ) {
    if ( t1 == t2 ) std::cout << "Same " << name << std::endl;
    else std::cout << "Don't Same" << name << std::endl;
}

#endif