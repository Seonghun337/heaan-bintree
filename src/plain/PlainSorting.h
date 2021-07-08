#ifndef PLAINSORTING_H_
#define PLAINSORTING_H_

#include "CyclicArray.h"
#include "vector"
#include "iostream"
#include "../MaskingGenerator.h"
#include "../PrintUtils.h"


class PlainSort {
public:
    PlainSort() {}
    ~PlainSort() {}
    void runPlainSorting(CyclicArray& ca, long log2n, bool=true);

    long sortingRec(CyclicArray& ca, double** mask, long logNum, long logJump, long loc, bool increase);

    void compAndSwap(CyclicArray& ca, double** mask, long loc, long dist, bool increase);

    void selfBitonicMerge(CyclicArray& ca, long log2n, double** mask, bool increase);

    void bitonicMerge(CyclicArray* ca, long log2n, long logNum);

    void bitonicMergeRec(CyclicArray* ca, long log2n, long start, long logNum, double** mask, double** mask2, bool increase);

    void compAndSwapTable(CyclicArray& ca, long logDataNum, long colNum, double* mask, double* maskRight, double* maskTable, double* maskTableRight, long dist, bool increase);
    
    void runPlainTableSorting(CyclicArray& ca, long log2n, long logDataNum, long colNum, bool=true);

    long plainSortingTableRecursion(CyclicArray& ca, long logDataNum, long colNum, long logNum, long logJump, long loc, double** mask, double** maskOther, double** maskTable, double** maskTableOther, bool increase);

    void bitonicTableMerge(CyclicArray* ca, long log2n, long logNum, long logDataNum, long colNum);
    
    void bitonicTableMergeRec(CyclicArray* ca, long log2n, long start, long logNum, long logDataNum, long colNum, double** maskCol, double*** maskInc, double*** maskDec, bool increase);

    void minMaxTable(CyclicArray& caLeft, CyclicArray& caRight, CyclicArray& caTableLeft, CyclicArray& caTableRight, long logDataNum, long colNum, double* maskRight, double* maskTableRight);

    void selfBitonicTableMerge(CyclicArray& ca, long log2n, long logDataNum, long colNum, double*** mask, bool increase);

};


#endif //! PLAINSORTING_H_
