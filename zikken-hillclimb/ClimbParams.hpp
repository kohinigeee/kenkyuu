#ifndef INCLUDE_CLIMBPARAMS_HPP
#define INCLUDE_CLIMBPARAMS_HPP

class ClimbParams {
    public:
    //st_s : 初期s
    //range: sの可動範囲
    //limt : 未更新解の最大連続数
    //exe : 実行回数
    int st_s, range, h, r;
    int limt, exe;
    int seed;

    double alpha;

    ClimbParams( int st_s, int range, int h, int r, int limt, int exe, int seed, double alpha ) : st_s(st_s), range(range), h(h), r(r), limt(limt), exe(exe) , seed(seed), alpha(alpha) {}
};

#endif 