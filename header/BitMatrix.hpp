#ifndef INCLUDE_BITMATRIX_HPP
#define INCLUDE_BITMATRIX_HPP

#include<iostream>
#include<vector>

using namespace std;
using bitmat_t = int;

class Bitmat {
    vector<bitmat_t> mat;
    int size;

    public:
    
    Bitmat ( int size ) : size(size) {
        int tmp = sizeof(bitmat_t)*8;
        int ele_size = size/tmp;
        if ( size%tmp != 0 ) ++ele_size;
        mat = vector<bitmat_t>(ele_size, 0);
    }

    int get(int i) {
        int bitsize = sizeof(bitmat_t)*8;

        int ele_no = i/(bitsize);
        int bit_no = i%(bitsize);

        int ans = (mat[ele_no]>>(bitsize-bit_no-1))&1;
        return  ans;
    }

    void change(int i, int v) {
        int bitsize = sizeof(bitmat_t)*8;

        int ele_no = i/(bitsize);
        int bit_no = i%(bitsize);
 
        if ( v == 0 ) {
            mat[ele_no] = mat[ele_no]&(~(1<<(bitsize-bit_no-1)));
        } else {
            mat[ele_no] = mat[ele_no]|(1<<(bitsize-bit_no-1));
        }
    }

};
#endif