#ifndef PARAMETER_H_
#define PARAMETER_H_

struct Parameter {
    long logN; long logQ;
    long logp; long logc;
    long log2n; long radix;
    long logq;
    long logT;
};

struct Param2 {
    long logN; long logQ;
    long logp; long logc;
    long log2n; long radix;
    long logq;
    long logT;
    long loga;
};



// Parameter bootstrapping_test_param1 = {16, 850, 30, 30, 6, 4, 35, 4};
// Parameter bootstrapping_test_param2 = {16, 850, 30, 30, 9, 8, 35, 4};
// Parameter bootstrapping_test_param3 = {16, 850, 30, 30, 12, 16, 35, 4};

#endif // !PARAMETER_H_
