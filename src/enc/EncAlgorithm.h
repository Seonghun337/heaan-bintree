#ifndef ENCALGORITHM_H_
#define ENCALGORITHM_H_

#include "../../HEAAN/src/HEAAN.h"
#include "../Parameter.h"
#include "../PrintUtils.h"

class EncAlgorithm {
public:
    EncAlgorithm() {}
    ~EncAlgorithm() {}

    void approxSqrt(Ciphertext& outCipher, const Ciphertext& inCipher, long iter, Parameter& param, Scheme& scheme);

    void minMax(Ciphertext& minCipher, Ciphertext& maxCipher, Ciphertext& input1, Ciphertext& input2, long iter, Parameter& param, Scheme scheme);

    void compAndSwap(Ciphertext& outCipher, const Ciphertext& inCipher, double* mask, long dist, long iter, Parameter& param, Scheme scheme);

};

#endif // !ENCALGORITHM_H_