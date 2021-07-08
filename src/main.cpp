#include "test/TestMask.h"
#include "test/TestPlain.h"
#include "test/TestEnc.h"
#include "test/TestBoot.h"
#include "test/TestSort.h"


int main() { 
    // ******************************
    // Parameters (long)
    // {logN, logQ, logp, logc, log2n, radix, logq, logT}
    // ******************************
    Parameter sortingTestParamSmall = {7, 1350, 40, 40, 4, 4, 50, 4};
    Parameter sortingTestParamSmall2 = {7, 1350, 40, 40, 6, 8, 50, 4};
    Parameter sortingTestParam1 = {12, 1350, 40, 40, 10, 32, 50, 5};
    Parameter sortingTestParamBig = {15, 650, 30, 30, 14, 128, 40, 5};
    Parameter sortingTestParamBig2_2 = {16, 1350, 40, 40, 10, 32, 50, 4};
    Parameter sortingTestParamBig2_3 = {16, 1350, 40, 40, 6, 8, 50, 4};
    Parameter sortingTestParamBig3 = {17, 1350, 40, 40, 16, 256, 50, 4};


    // ******************************
    // *** Test Maskings
    // ******************************
    long logn = 5;
    long logDataNum = 2;
    long colNum = 1;
    // TestMask::showMasking(logn, true);
    // TestMask::showMasking(logn, false);
    // TestMask::showMaskingOther(logn, true);
    // TestMask::showBitonicMergeMasking(logn, true);
    // TestMask::showBitonicMergeMasking(logn, false);
    // TestMask::showColNumMasking(logn, logDataNum, colNum, true);
    // TestMask::showTableMergeMasking(logn, logDataNum, colNum, true);
    // TestMask::showTableMergeMaskingOther(logn, logDataNum, colNum, true);
    // TestMask::showTableMergeMasking(logn, logDataNum, colNum, false);
    // TestMask::showTableMergeMaskingOther(logn, logDataNum, colNum, false);
    // TestMask::showTableMasking(logn, logDataNum, true);
    // TestMask::showTableMaskingBy(logn, logDataNum, 0, true);
    // TestMask::showTableMaskingOther(logn + logDataNum, logDataNum, 0, true);
    // TestMask::showReverseMasking(logn, true);
    // TestMask::showReverseMaskingRight(logn, true);

    // TestMask::showReverseMasking(logn, false);
    // TestMask::showReverseMaskingRight(logn, false);

    // ******************************
    // *** Test PlainSort
    // ******************************
    
    // TestPlain::plainSort(5);
    // TestPlain::bitonicMerge(4, 4);
    // TestPlain::plainTableSort(4, 1, 0, true);
    // TestPlain::plainTableSort(4, 1, 0, false);

    // ******************************
    // *** Test algorithms for encrypted data with Bootstrapping
    // ******************************
    // ******************************
    // TestBoot::approxSqrt(sortingTestParam1, 15);
    // TestBoot::approxInverse(sortingTestParamSmall, 5);
    // TestBoot::approxComp(sortingTestParamSmall, 5, 5);
    // TestBoot::minMax(sortingTestParamSmall, 15);
    // TestBoot::compAndSwap(sortingTestParamSmall, 13);
    // TestBoot::reverse(sortingTestParamSmall);
    // TestBoot::compAndSwapTable(sortingTestParamBig2, 2, 0, 5, 8);
    // TestBoot::halfCleaner(sortingTestParamSmall, 10);

    // ******************************
    // *** Check Parameters
    // ******************************
    // TestBoot::bootstrapping(sortingTestParamSmall);
    // TestEnc::compAndSwap(sortingTestParam1, 10);

    // ******************************
    // *** Test EncSorting
    // ******************************
    TestSort::sort(sortingTestParamBig2_3, 12);
    //TestSort::merge(sortingTestParamBig2_2, 12, 2);
    // TestSort::sortAndMerge(sortingTestParamSmall, 15, );

    //TestSort::tableSort(sortingTestParamBig2_2, 4, 0, 5, 5, true);
    // TestSort::tableMerge(sortingTestParamSmall, 2, 2, 0, 5, 5);    
    // TestSort::bitonicSort(sortingTestParamSmall, 13);

    return 0;
}
