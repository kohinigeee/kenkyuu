#ifndef INCLUDE_EXECEPTION_HPP
#define INCLUDE_EXECEPTION_HPP

#include<string>

using namespace std;

class IregalValueException {
    string msg;

    public:
    IregalValueException(string msg = "" ) : msg(msg) {
    }

    string getMesage() { return msg; }
};


class IregalManuplateException {
    string msg;

    public:
    IregalManuplateException(string msg = "" ) : msg(msg) {
    }

    string getMesage() { return msg; }
};

#endif