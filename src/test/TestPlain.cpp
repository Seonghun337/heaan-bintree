#include "TestPlain.h"


void TestPlain::plainSort(long logn, bool increase) {
    CyclicArray ca;
    ca.randomGen(1 << logn);

    cout << "Before sorting" << endl;
    ca.printAsVector();
    
    cout << "After sorting" << endl;
    PlainSort plainSort;
    plainSort.runPlainSorting(ca, logn, increase);
    ca.printAsVector();
}

void TestPlain::bitonicMerge(long log2n, long logNum) {
    PlainSort plainSort;
    CyclicArray* ca = new CyclicArray[1 << logNum];
    for(int i = 0; i < (1 << logNum); i++) {
        double* mvec = EvaluatorUtils::randomRealArray(1 << log2n);
        ca[i] = CyclicArray(mvec, 1 << log2n);
        plainSort.runPlainSorting(ca[i], log2n, (i % 2 == 0));
        ca[i].printAsVector();
        cout << "==" << endl;
    }

    plainSort.bitonicMerge(ca, log2n, logNum);
    for(int i = 0; i < (1 << logNum); i++) {
        ca[i].printAsVector();
        cout << "===" << endl;
    }
    
}

void TestPlain::plainTableSort(long logn, long logDataNum, long colNum, bool increase) {
    cout << "increase " << increase << endl;
    CyclicArray ca;
    ca.randomGen(1 << logn);
    double* mvec = ca.getArray();
    CyclicArray plain(ca);
    PlainSort plainSort;
    plainSort.runPlainTableSorting(ca, logn, logDataNum, colNum, increase);
    double* dvec = ca.getArray();
    
    PrintUtils::printArraysWithDataNum(mvec, dvec, 1 << logn, logDataNum, colNum);
}

void TestPlain::bitonicTableMerge(long log2n, long logNum, long logDataNum, long colNum) {
    PlainSort plainSort;
    
    CyclicArray* ca = new CyclicArray[1 << logNum];
    for(int i = 0; i < (1 << logNum); i++) {
        double* mvec = EvaluatorUtils::randomRealArray(1 << log2n);
        ca[i] = CyclicArray(mvec, 1 << log2n);
        plainSort.runPlainSorting(ca[i], log2n, (i % 2 == 0));
        ca[i].printAsVector();
        cout << "==" << endl;
    }

    plainSort.bitonicMerge(ca, log2n, logNum);
    for(int i = 0; i < (1 << logNum); i++) {
        ca[i].printAsVector();
        cout << "===" << endl;
    }
    
}