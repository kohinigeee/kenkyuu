#ifndef INCLUDE_DEBUG_HTTP
#define INCLUDE_DEBUG_HTPP

#define DEB() if ( is_deb ) 

bool is_deb = true;

void debug_on() {
    is_deb = true;
}

void debug_off() {
    is_deb = false;
}

#endif