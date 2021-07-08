#ifndef TESTPLAIN_H_
#define TESTPLAIN_H_

#include "../plain/CyclicArray.h"
#include "../plain/PlainSorting.h"
#include "../HEAAN/src/EvaluatorUtils.h"
#include "../PrintUtils.h"

class TestPlain {
public:
    static void plainSort(long logn, bool increase);

    static void bitonicMerge(long log2n, long logNum);

    static void bitonicTableMerge(long log2n, long logNum, long logDataNum, long colNum);

    static void plainTableSort(long logn, long logDataNum, long colNum, bool=true);
};


#endif // !TESTPLAIN_H_