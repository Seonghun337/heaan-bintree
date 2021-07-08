#ifndef ENCSORTING_H_
#define ENCSORTING_H_

#include "BootAlgorithm.h"
#include "../plain/PlainSorting.h"
#include "../Parameter.h"
#include "../MaskingGenerator.h"

class EncSorting {
private:
    Parameter param;
    long sqrtIter;
    long invIter;
    long compIter;
    BootAlgo bootAlgo;

public:
    EncSorting(Parameter _param, long _sqrtIter) : param(_param), sqrtIter(_sqrtIter) {}
    EncSorting(Parameter _param, long _invIter, long _compIter) : param(_param), invIter(_invIter), compIter(_compIter) {}
    ~EncSorting() {}
    
    void runEncSorting(Ciphertext& cipher, BootScheme& scheme, Ring& ring, BootHelper& bootHelper, bool=true);
    void runEncSortingDec(Ciphertext& cipher, BootScheme& scheme, Ring& ring, BootHelper& bootHelper, bool increase, SecretKey sk);

    void runEncTableSorting(Ciphertext& cipher, long logDataNum, long colNum, BootScheme& scheme, Ring& ring, BootHelper& bootHelper, SecretKey& secretKey, bool=true);

    long sortingRecursion(Ciphertext& cipher, long logNum, long logJump, long loc, double** mask, BootScheme& scheme, Ring& ring, BootHelper& bootHelper);
    long sortingRecursion(Ciphertext& cipher, long logNum, long logJump, long loc, double** mask, BootScheme& scheme, Ring& ring, BootHelper& bootHelper, SecretKey sk, PlainSort ps);
    
    long sortingTableRecursion(Ciphertext& cipher, long logDataNum, long colNum, long logNum, long logJump, long loc,
                                    double** mask, double** maskRight, double** maskTable, double** maskTableRight,
                                    BootScheme& scheme, Ring& ring, BootHelper& bootHelper, SecretKey& secretKey, PlainSort ps);

    void compAndSwapBothWithDec(Ciphertext& cipher, long logJump, long loc, double** mask, BootScheme& scheme, Ring& ring, BootHelper& bootHelper, SecretKey sk, PlainSort ps);
    void CompAndSwapTableBothWithDec(Ciphertext& cipher, long logDataNum, long colNum, long dist,
                                    double* mask, double* maskRight, double* maskTable, double* maskTableRight,
                                    BootScheme& scheme, Ring& ring, BootHelper& bootHelper, SecretKey& secretKey, PlainSort ps);

    void bitonicMerge(Ciphertext* cipher, long logNum, BootScheme& scheme, Ring& ring, BootHelper& bootHelper);
    void bitonicMergeRec(Ciphertext* cipher, long start, long logNum, double** maskIncrease, double** maskDecrease, BootScheme& scheme, Ring& ring, BootHelper& bootHelper, bool increase);

    void bitonicTableMerge(Ciphertext* cipher, long logNum, long logDataNum, long colNum, BootScheme& scheme, Ring& ring, BootHelper& bootHelper, SecretKey& sk);
    void bitonicTableMergeRec(Ciphertext* cipher, long start, long logNum, long logDataNum, long colNum, ZZ** maskminMaxTablePoly, double*** maskInc, double*** maskDec, BootScheme& scheme, Ring& ring, BootHelper& bootHelper, SecretKey& sk, bool increase);
    
    void reverseHalf(Ciphertext* cipher, long logNum, BootScheme& scheme, Ring& ring, BootHelper& bootHelper);

    void runBitonicSort(Ciphertext& cipher, BootScheme& scheme, Ring& ring, BootHelper& bootHelper, bool increase);

    void runBitonicSortDec(Ciphertext& cipher, BootScheme& scheme, Ring& ring, BootHelper& bootHelper, bool increase, SecretKey sk);
};

#endif // !ENCSORTING_H_