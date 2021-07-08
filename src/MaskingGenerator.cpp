#include "MaskingGenerator.h"


MaskingGenerator::MaskingGenerator(long _log2n, bool _increase)  {
    log2n = _log2n;
    length = 1 << log2n;
    increase = _increase;
    logDataNum = 0;
    dataNum = 1;
    colNum = -1;
    setMaskNum();
}

MaskingGenerator::MaskingGenerator(long _log2n, long _logDataNum, bool _increase) {
    log2n = _log2n;
    length = 1 << log2n;
    increase = _increase;
    logDataNum = _logDataNum;
    dataNum = 1 << logDataNum;
    colNum = -1;
    setMaskNum();
}

MaskingGenerator::MaskingGenerator(long _log2n, long _logDataNum, long _colNum, bool _increase) {
    log2n = _log2n;
    length = 1 << log2n;
    increase = _increase;
    logDataNum = _logDataNum;
    dataNum = 1 << logDataNum;
    colNum = _colNum;
    setMaskNum();
}

MaskingGenerator::~MaskingGenerator() {
    if(mask != NULL) delete[] mask;
}

void MaskingGenerator::setMaskNum() {
    if (colNum == -1) {
        maskNum = log2n * (log2n + 1) / 2;
    } else {
        maskNum = (log2n - logDataNum) * (log2n - logDataNum + 1) / 2;
    }
    
    mask = new double*[maskNum];
    for(int i = 0; i < maskNum; i++) {
        mask[i] = new double[length]();
    }
}

void MaskingGenerator::printMask(double** mask, long maskNum) {
    for(int i = 0; i < maskNum; i++){
        std::cout << "mask[" << i << "] = [";
        for(int j = 0; j < length; j++) {
            std::cout << mask[i][j] << ", ";
        }
        std::cout << "]" << std::endl;        
    }
    std::cout << std::endl;
}

double** MaskingGenerator::getMasking() {
    generateMaskingRec(log2n, 0, 0);
    return mask;
}

double** MaskingGenerator::getMaskingOther() {
    generateMaskingRecOther(log2n, 0, 0);
    return mask;
}

double** MaskingGenerator::getBitonicMergeMasking() {
    for(int i = 0; i < log2n - logDataNum; i++) {
        generateBitonicMergeMasking(i);
    }
    return mask;
}

double** MaskingGenerator::getBitonicMergeMaskingOther() {
    for(int i = 0; i < log2n - logDataNum; i++) {
        generateBitonicMergeMaskingOther(i);
    }
    return mask;
}

double** MaskingGenerator::getColNumMasking() {
    mask = new double*[2];
    mask[0] = new double[length];
    for (int i = 0; i < length; i++) {
        mask[0][i] = 1.;
    }
    mask[1] = new double[length];
    for (int i = 0; i < length; i++) {
        if (i % dataNum == colNum) {
            mask[1][i] = 1.;
        } else {
            mask[1][i] = 0.;
        }
    }
    return mask;
}

double** MaskingGenerator::getReverseMasking(long level) {
    mask = new double*[level];
    for (int i = 0; i < level; i++) {
        mask[i] = new double[length];
        for (int j = 0; j < length; j++) {
            mask[i][j] = 0.;
        }
    }
    
    for (int i = 0; i < level; i++) {
        generateReverseMasking(level, i);
    }
    return mask;
}

double** MaskingGenerator::getReverseMaskingRight(long level) {
    mask = new double*[level];
    for (int i = 0; i < level; i++) {
        mask[i] = new double[length];
        for (int j = 0; j < length; j++) {
            mask[i][j] = 0.;
        }
    }
    
    for (int i = 0; i < level; i++) {
        generateReverseMaskingRight(level, i);
    }
    return mask;
}

void MaskingGenerator::generateBitonicMergeMasking(long num) {
    for(int j = 0; j < (1 << num); j++) {
        for(int k = 0; k < (1 << (log2n - 1 - num)); k++) {
            if (colNum == -1 || k % dataNum == colNum) {
                long loc = j * (1 << (log2n - num)) + k;
                if (!increase) {
                    loc = length - 1 - loc;
                    if (colNum != -1) {
                        loc = loc + 2 * colNum - dataNum + 1;
                    }
                }
                mask[num][loc] = 1; 
            }
        }
    }
}

void MaskingGenerator::generateBitonicMergeMaskingOther(long num) {
    for(int j = 0; j < (1 << num); j++) {
        for(int k = 0; k < (1 << (log2n - 1 - num)); k++) {
            if (colNum == -1 || k % dataNum == colNum) {
                long loc = j * (1 << (log2n - num)) + k + (1 << (log2n - 1 - num));
                if (!increase) {
                    loc = length - 1 - loc;
                    if (colNum != -1) {
                        loc = loc + 2 * colNum - dataNum + 1;
                    }
                }
                mask[num][loc] = 1; 
            }
        }
    }
}

void MaskingGenerator::generateMaskingComparison(long loc, long jump) {
    jump *= dataNum;
    long repeat = length / (jump * 2);
    
    for(int i = 0; i < repeat; i++) {
        for(int j = 0; j < jump; j++) {
            if (colNum == -1 || j % dataNum == colNum) {
                long tmp = i * jump * 2 + j;
                if(!increase) {
                    tmp = length - 1 - tmp;
                    if (colNum != -1) {
                        tmp = tmp + 2 * colNum - dataNum + 1;
                    }
                }
                mask[loc][tmp] = 1;
            }            
        }
    }
}

void MaskingGenerator::generateMaskingMerge(long loc, long num, long jump) {
    jump *= dataNum;
    long repeat = length / (jump * num);
    
    for(int i = 0; i < repeat; i++) {
        for(int j = 0; j < jump; j++) {
            if(colNum == -1 || j % dataNum == colNum) {
                for(int k = 0; k < num / 2 - 1; k++) {
                    long tmp = i * jump * num + (2 * k + 1) * jump + j;
                    if(!increase) {
                        tmp = length - 1 - tmp;
                        if (colNum != -1) {
                            tmp = tmp + 2 * colNum - dataNum + 1;
                        }
                    } 
                    mask[loc][tmp] = 1;    
                }            
            }
            
        }        
    }
}

long MaskingGenerator::generateMaskingRec(long logNum, long logJump, long loc) {
    if (logNum == 1) {
        generateMaskingComparison(loc, 1 << logJump);
    } else {
        if (logJump == 0) {
            loc = generateMaskingRec(logNum - 1, logJump, loc);
        }
        loc = generateMaskingRec(logNum - 1, logJump + 1, loc);
        generateMaskingMerge(loc, 1 << logNum, 1 << logJump);
    }
    return loc + 1;
}

long MaskingGenerator::generateMaskingRecOther(long logNum, long logJump, long loc) {
    if (logNum == 1) {
        generateMaskingComparisonOther(loc, 1 << logJump);
    } else {
        if (logJump == 0) {
            loc = generateMaskingRecOther(logNum - 1, logJump, loc);
        }
        loc = generateMaskingRecOther(logNum - 1, logJump + 1, loc);
        generateMaskingMergeOther(loc, 1 << logNum, 1 << logJump);
    }
    return loc + 1;
}

void MaskingGenerator::generateMaskingComparisonOther(long loc, long jump) {
    jump *= dataNum;
    long repeat = length / (jump * 2);
    
    for(int i = 0; i < repeat; i++) {
        for(int j = 0; j < jump; j++) {
            if (colNum == -1 || j % dataNum == colNum) {
                long tmp = i * jump * 2 + j + jump;
                if(!increase) {
                    tmp = length - 1 - tmp;
                    if (colNum != -1) {
                        tmp = tmp + 2 * colNum - dataNum + 1;
                    }
                }
                mask[loc][tmp] = 1;
            }            
        }
    }
}

void MaskingGenerator::generateMaskingMergeOther(long loc, long num, long jump) {
    jump *= dataNum;
    long repeat = length / (jump * num);
    
    for(int i = 0; i < repeat; i++) {
        for(int j = 0; j < jump; j++) {
            if(colNum == -1 || j % dataNum == colNum) {
                for(int k = 0; k < num / 2 - 1; k++) {
                    long tmp = i * jump * num + (2 * k + 1) * jump + j + jump;
                    if(!increase) {
                        tmp = length - 1 - tmp;
                        if (colNum != -1) {
                            tmp = tmp + 2 * colNum - dataNum + 1;
                        }
                    } 
                    mask[loc][tmp] = 1;    
                }            
            }
            
        }        
    }
}

void MaskingGenerator::generateReverseMasking(long level, long num) {
    long repeat = length / (1 << (level + 1));
    long subRepeat = 1 << num;
    for (int i = 0; i < repeat; i++) {
        for (int j = 0; j < subRepeat; j++) {
            for (int k = 0; k < (1 << (level - num - 1)); k++) {
                long tmp = i * (1 << (level + 1)) + (1 << level) + j * (1 << (level - num)) + k;
                if(!increase) tmp = length - 1 - tmp;
                mask[num][tmp] = 1.;
            }
        }
    }
}

void MaskingGenerator::generateReverseMaskingRight(long level, long num) {
    long repeat = length / (1 << (level + 1));
    long subRepeat = 1 << num;
    for (int i = 0; i < repeat; i++) {
        for (int j = 0; j < subRepeat; j++) {
            for (int k = 0; k < (1 << (level - num - 1)); k++) {
                long tmp = i * (1 << (level + 1)) + (1 << level) + j * (1 << (level - num)) +  (1 << (level - num - 1)) + k;
                if(!increase) tmp = length - 1 - tmp;
                mask[num][tmp] = 1.;
            }
        }
    }
}