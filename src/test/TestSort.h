#ifndef TESTSORT_H_
#define TESTSORT_H_

#include "../enc/EncSorting.h"
#include "../plain/PlainSorting.h"
#include <algorithm>

class TestSort {
public:
    static void sort(Parameter param, long iter, bool=true);
    static void tableSort(Parameter param, long logDataNum, long colNum, long invIter, long compIter, bool=true);
    static void merge(Parameter param, long iter, long logNum);
    static void tableMerge(Parameter param, long logNum, long logDataNum, long colNum, long invIter, long compIter);
    static void sortAndMerge(Parameter param, long iter, long logNum);
    static void bitonicSort(Parameter param, long iter, bool=true);

};



#endif // !TESTSORT_H_