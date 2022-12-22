#ifndef INCLUDE_RATIONAL_HPP
#define INCLUDE_RATIONAL_HPP

#include<iostream>
#include<utility>

using namespace std;

class Rational {
    long long numera; //分子
    long long denomi; //分母

    public:
    Rational ( long long numera, long long denomi ) : numera(numera), denomi(denomi){
        if ( denomi == 0 ) {
            cout << "[Error] denomi value == 0" << endl;
        }
    };

    Rational yakubun() {
        long long gcdv = gcd(numera, denomi);
        return Rational(numera/gcdv, denomi/gcdv);
    }

    pair<Rational, Rational> tubun (const Rational& r1 ) const {
        long long lcmv = lcm(denomi, r1.denomi);
        Rational ans1 = Rational(numera*(lcmv/denomi), lcmv);
        Rational ans2 = Rational(r1.numera*(lcmv/r1.denomi), lcmv);

        return make_pair(ans1, ans2);
    }

    friend bool operator<(const Rational&, const Rational& );
    friend Rational operator+(const Rational&, const Rational&);
    friend Rational operator-(const Rational&, const Rational& );
    friend ostream& operator<<(ostream& os, const Rational& r);

    private:
    long long static gcd ( long long a, long long b);
    long long static lcm ( long long a, long long b);
};

bool operator<( const Rational& r1, const Rational& r2 ) {
    pair<Rational, Rational> prr = r1.tubun(r2);
    return r1.numera < r2.numera;
}

Rational operator+( const Rational& r1, const Rational& r2 ) {
    pair<Rational, Rational> prr = r1.tubun(r2);
    Rational ans = Rational(prr.second.numera+prr.first.numera, prr.first.denomi);
    return ans.yakubun();
}

Rational operator-( const Rational& r1, const Rational& r2 ) {
    pair<Rational, Rational> prr = r2.tubun(r1);
    Rational ans = Rational(prr.second.numera-prr.first.numera, prr.first.denomi);
    return ans.yakubun();
}

long long Rational::gcd ( long long a, long long b ) {
    long long r;

    while ( r = a%b ) {
        a = b;
        b = r;
    }
    return b;
}

long long Rational::lcm ( long long a, long long b ) {
    long long gcdv = gcd(a,b);
    return a*(b/gcdv);
}

ostream& operator<<(ostream& os, const Rational& r ) {
    os << r.numera << "/" << r.denomi;
    return os;
}

#endif