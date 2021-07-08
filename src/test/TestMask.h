#ifndef TESTMASK_H_
#define TESTMASK_H_

#include "../MaskingGenerator.h"

class TestMask {
public:
    static void showMasking(long log2n, bool increase);

    static void showMaskingOther(long log2n, bool increase);

    static void showTableMasking(long log2n, long logDataNum, bool increase);

    static void showTableMaskingBy(long log2n, long logDataNum, long colNum, bool increase);

    static void showTableMaskingOther(long log2n, long logDataNum, long colNum, bool increase);

    static void showBitonicMergeMasking(long log2n, bool increase);

    static void showColNumMasking(long log2n, long logDataNum, long colNum, bool increase);

    static void showTableMergeMasking(long log2n, long logDataNum, long colNum, bool increase);

    static void showTableMergeMaskingOther(long log2n, long logDataNum, long colNum, bool increase);    

    static void showReverseMasking(long log2n, bool increase);    
    static void showReverseMaskingRight(long log2n, bool increase);    
};


#endif // !TESTMASK_H_