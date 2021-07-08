#ifndef MASKINGGENERATOR_H_
#define MASKINGGENERATOR_H_

#include "iostream"

/*
 * class MaskingGenerator
 *      generate masking vectors for Batcher's Odd-Even Sort
 */
class MaskingGenerator
{
private:
    double **mask;
    long log2n;
    long length;
    bool increase;
    long logDataNum;
    long dataNum;
    long colNum;
    long maskNum;

public:
    MaskingGenerator(long _log2n, bool = true);
    MaskingGenerator(long _log2n, long _logDataNum, bool = true);
    MaskingGenerator(long _log2n, long _logDataNum, long _colNum, bool=true);
    ~MaskingGenerator();

    // set Number of masking vectors
    void setMaskNum();
    void printMask(double** mask, long maskNum);

    // ********************
    // *** get functions
    // ********************
    double **getMasking();
    double **getMaskingOther();
    // double **getTableMasking();
    double **getBitonicMergeMasking();
    double **getBitonicMergeMaskingOther();
    double **getColNumMasking();
    double **getReverseMasking(long level);
    double **getReverseMaskingRight(long level);

    // ********************
    // *** Odd-Even maskings
    // ********************
    long generateMaskingRec(long logNum, long logJump, long loc);
    void generateMaskingComparison(long loc, long jump);
    void generateMaskingMerge(long loc, long num, long jump);

    long generateMaskingRecOther(long logNum, long logJump, long loc);
    void generateMaskingComparisonOther(long loc, long jump);
    void generateMaskingMergeOther(long loc, long num, long jump);

    // ********************
    // *** Bitonic Maskings
    // ********************
    void generateBitonicMergeMasking(long num);
    void generateBitonicMergeMaskingOther(long num);
    void generateReverseMasking(long level, long num);
    void generateReverseMaskingRight(long level, long num);
};

#endif // !MASKINGGENERATOR_H_