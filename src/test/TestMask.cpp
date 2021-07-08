#include "TestMask.h"


void TestMask::showMasking(long log2n, bool increase) {
    MaskingGenerator mg(log2n, increase);
    double** mask = mg.getMasking();

    long maskNum = log2n * (log2n + 1) / 2;
    mg.printMask(mask, maskNum);
}

void TestMask::showMaskingOther(long log2n, bool increase) {
    MaskingGenerator mg(log2n, increase);
    double** mask = mg.getMaskingOther();

    long maskNum = log2n * (log2n + 1) / 2;
    mg.printMask(mask, maskNum);
}

void TestMask::showTableMasking(long log2n, long logDataNum, bool increase) {
    MaskingGenerator mg(log2n, logDataNum, increase);
    // double** mask = mg.getTableMasking();
    double** mask = mg.getMasking();

    long logNum = log2n - logDataNum;
    long maskNum = logNum * (logNum + 1) / 2;
    mg.printMask(mask, maskNum);
}

void TestMask::showTableMaskingBy(long log2n, long logDataNum, long colNum, bool increase) {
    MaskingGenerator mg(log2n, logDataNum, colNum, increase);
    // double** mask = mg.getTableMaskingBy(colNum);
    double** mask = mg.getMasking();

    long logNum = log2n - logDataNum;
    long maskNum = logNum * (logNum + 1) / 2;
    mg.printMask(mask, maskNum);    
}

void TestMask::showTableMaskingOther(long log2n, long logDataNum, long colNum, bool increase) {
    MaskingGenerator mg(log2n, logDataNum, colNum, increase);
    double** mask = mg.getMaskingOther();

    long logNum = log2n - logDataNum;
    long maskNum = logNum * (logNum + 1) / 2;
    mg.printMask(mask, maskNum);    
}

void TestMask::showColNumMasking(long log2n, long logDataNum, long colNum, bool increase) {
    MaskingGenerator mg(log2n, logDataNum, colNum, increase);
    double** mask = mg.getColNumMasking();
    
    mg.printMask(mask, 2);    
}

void TestMask::showBitonicMergeMasking(long log2n, bool increase) {
    MaskingGenerator mg(log2n, increase);
    double** mask = mg.getBitonicMergeMasking();
    
    mg.printMask(mask, log2n);
}

void TestMask::showTableMergeMasking(long log2n, long logDataNum, long colNum, bool increase) {
    MaskingGenerator mg(log2n, logDataNum, colNum, increase);
    double** mask = mg.getBitonicMergeMasking(); 
    mg.printMask(mask, log2n - logDataNum);
}

void TestMask::showTableMergeMaskingOther(long log2n, long logDataNum, long colNum, bool increase) {
    MaskingGenerator mg(log2n, logDataNum, colNum, increase);
    double** mask = mg.getBitonicMergeMaskingOther(); 
    mg.printMask(mask, log2n - logDataNum);
}

void TestMask::showReverseMasking(long log2n, bool increase) {
    MaskingGenerator mg(log2n, increase);
    long level = 3;
    double** mask = mg.getReverseMasking(level); 
    mg.printMask(mask, level);
}

void TestMask::showReverseMaskingRight(long log2n, bool increase) {
    MaskingGenerator mg(log2n, increase);
    long level = 3;
    double** mask = mg.getReverseMaskingRight(level); 
    mg.printMask(mask, level);
}