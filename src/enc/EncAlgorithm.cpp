#include "EncAlgorithm.h"


// ************************************
// *** class EncAlgorithm
// ************************************

void EncAlgorithm::approxSqrt(Ciphertext& outCipher, const Ciphertext& inCipher, long iter, Parameter& param, Scheme& scheme) {
    PrintUtils::nprint("start EncAlgo::sqrt", WANT_TO_PRINT);
    Ciphertext a = inCipher;
    Ciphertext b = inCipher;
    scheme.addConstAndEqual(b, -1.0, param.logp);
    // one iteration : 2logp 
    // total = 2 * d * logp
    Ciphertext dummy;
    for(int i = 0; i < iter; i++) {
        PrintUtils::nprint(to_string(i) + "/" + to_string(iter - 1) + "th iteration", WANT_TO_PRINT);
        
        // make dummy = 1 - b / 2
        dummy = scheme.divByPo2(b, 1); // b - 1
        scheme.negateAndEqual(dummy); 
        scheme.addConstAndEqual(dummy, 1.0, param.logp); // dummy - 1

        // Update a
        // a <- a * (1 - b / 2)
        scheme.modDownToAndEqual(a, dummy.logq); // a - 1
        scheme.multAndEqual(a, dummy); 
        scheme.reScaleByAndEqual(a, param.logp); // a - logp + 1

        // make dummy = (b - 3) / 4
        dummy = scheme.addConst(b, -3.0, param.logp);
        scheme.divByPo2AndEqual(dummy, 2); // dummy - 3

        //update b
        // b<- b * b * (b - 3) / 4
        scheme.squareAndEqual(b);
        scheme.reScaleByAndEqual(b, param.logp); // b - logp
        scheme.modDownToAndEqual(dummy, b.logq);
        scheme.multAndEqual(b, dummy);
        scheme.reScaleByAndEqual(b, param.logp); // b - 2logp
        scheme.modDownToAndEqual(a, b.logq);

        PrintUtils::nprint("logq = " + to_string(a.logq), WANT_TO_PRINT);
    }

    outCipher = a;  
}
 
void EncAlgorithm::minMax(Ciphertext& minCipher, Ciphertext& maxCipher, Ciphertext& input1, Ciphertext& input2, long iter, Parameter& param, Scheme scheme) {
    Ciphertext x = scheme.add(input1, input2);
    Ciphertext y = scheme.sub(input1, input2);
    scheme.divByPo2AndEqual(x, 1); // x - logp + 1
    scheme.divByPo2AndEqual(y, 1); // y - logp + 1
    
    scheme.squareAndEqual(y);
    scheme.reScaleByAndEqual(y, param.logp); // y - logp + 1

    Ciphertext sqrtCipher;
    // sqrtCipher - (2 * iter + 1) * logp + 1
    approxSqrt(sqrtCipher, y, iter, param, scheme);

    scheme.modDownToAndEqual(x, sqrtCipher.logq);

    maxCipher = scheme.add(x, sqrtCipher);
    minCipher = scheme.sub(x, sqrtCipher);
}

void EncAlgorithm::compAndSwap(Ciphertext& outCipher, const Ciphertext& inCipher, double* mask, long dist, long iter, Parameter& param, Scheme scheme) {
    Ciphertext a = inCipher;
    long n = a.n;
    long logQ = a.logq;
    Ciphertext maskCipher = scheme.encrypt(mask, n, param.logp, param.logQ);
    Ciphertext dummy = scheme.mult(a, maskCipher);
    scheme.reScaleByAndEqual(dummy, param.logp);
    scheme.modDownByAndEqual(a, param.logp);
    scheme.subAndEqual(a, dummy);
    scheme.rightRotateFastAndEqual(dummy, dist);
    Ciphertext min, max;
    minMax(min, max, a, dummy, iter, param, scheme);
    scheme.leftRotateFastAndEqual(min, dist);
    scheme.addAndEqual(max, min);

    outCipher = max;
}

