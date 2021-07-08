#include "PrintUtils.h"

 void PrintUtils::parameter(Parameter param, std::string str) {
    long n = 1 << param.log2n;

    std::cout << "\n***************************" << std::endl;
    std::cout << "Parameter for " << str << std::endl;
    std::cout << "logN = " << param.logN << ", logQ = " << param.logQ << ", logp = " << param.logp << ", logc = " << param.logc << std::endl;
    std::cout << "slots = " << n << ", radix = " << param.radix << ", logq = " << param.logq << ", logT = " << param.logT << std::endl;
    std::cout << "***************************" << std::endl;
    std::cout << std::endl;
}

void PrintUtils::averageDifference(double* a1, std::complex<double>* a2, long n) {
	double avg = 0.;
	for(long i = 0; i < n; i++) {
		avg += abs(a1[i] - a2[i].real());
	}
	avg /= n;
	std::cout << "log2(avg of error) = " << log2(avg) << std::endl;
}

void PrintUtils::averageDifference(std::complex<double>* a1, std::complex<double>* a2, long n) {
    double avg = 0.;
	for(long i = 0; i < n; i++) {
		avg += abs(a1[i].real() - a2[i].real());
	}
	avg /= n;
	std::cout << "log2(avg of error) = " << log2(avg) << std::endl;
}

void PrintUtils::printSingleArray(std::string str, double* array, long n) {
    for (int i = 0; i < n; i++) {
        std::cout << str << "[" << i << "] : " << array[i] << std::endl;
    }
}

void PrintUtils::printSingleArray(std::string str, complex<double>* array, long n) {
    for (int i = 0; i < n; i++) {
        std::cout << str << "[" << i << "] = " << array[i] << std::endl;
    }
}

void PrintUtils::printSingleArraySmall(std::string str, double* array, long n) {
    if(n <= 1024) {
        PrintUtils::printSingleArray(str, array, n);
    } else {
        for(int i = 0; i < n; i++) {
            if(i % 1000 == 0) {
                std::cout << str << "[" << i << "] = " << array[i] << std::endl;
            }
        }
    }
}

void PrintUtils::printSingleArraySmall(std::string str, complex<double>* array, long n) {
    if(n <= 1024) {
        PrintUtils::printSingleArray(str, array, n);
    } else {
        for(int i = 0; i < n; i++) {
            if(i % 1000 == 0) {
                std::cout << str << "[" << i << "] = " << array[i] << std::endl;
            }
        }
    }
}

void PrintUtils::printSingleMatrix(std::string str, double** matrix, long row, long col) {
    for(int i = 0; i < row; i++) {
        std::cout << str << "[" << i << "] = [";
        for (int j = 0; j < col; j++) {
            std::cout << matrix[i][j] << ", ";
        }
        std::cout << "]" << std::endl;
    }
}

void PrintUtils::printArrays(double* a1, std::complex<double>* a2, long n) {
    for(int i = 0; i < n; i++) {
        std::cout << i << " : " << a1[i] << " // " << a2[i] << std::endl;
    }
}

void PrintUtils::printFewArrays(double* a1, std::complex<double>* a2, long n) {
    for(int i = 0; i < n; i++) {
        if(n < 1000 || i % 1000 == 0) {
            std::cout << i << " : " << a1[i] << " // " << a2[i] << std::endl;
        }       
    }
}

void PrintUtils::printFewArrays(std::complex<double>* a1, std::complex<double>* a2, long n) {
    for(int i = 0; i < n; i++) {
        if(n < 1000 || i % 1000 == 0) {
            std::cout << i << " : " << a1[i] << " // " << a2[i] << std::endl;
        }       
    }
}

void PrintUtils::printArraysWithDataNum(double* a1, double* a2, long n, long logDataNum, long colNum) {
    long dataNum = 1 << logDataNum;
    for(int i = 0; i < n; i++) {
        if(i % dataNum == 0) {
            std::cout << "-----------------------------" << endl;
        }
        if(i % dataNum == colNum) {
            std::cout << "<<";
        }
        std::cout << i << " : " << a1[i] << " // " << a2[i];
        if(i % dataNum == colNum) {
            std::cout << ">>";
        }
        std:: cout << std::endl;
    }
}

void PrintUtils::printArraysWithDataNum(double* a1, std::complex<double>* a2, long n, long logDataNum, long colNum) {
    long dataNum = 1 << logDataNum;
    for(int i = 0; i < n; i++) {
        if(i % dataNum == 0) {
            std::cout << "-----------------------------" << endl;
        }
        if(i % dataNum == colNum) {
            std::cout << "<<";
        }
        std::cout << i << " : " << a1[i] << " // " << a2[i].real();
        if(i % dataNum == colNum) {
            std::cout << ">>";
        }
        std:: cout << std::endl;
    }
}

void PrintUtils::printArrays(std::complex<double>* a1, std::complex<double>* a2, long n) {
    for(int i = 0; i < n; i++) {
        std::cout << i << " : " << a1[i] << " // " << a2[i] << std::endl;
    }
}

void PrintUtils::printArraysWithDataNum(std::complex<double>* a1, std::complex<double>* a2, long n, long logDataNum) {
    long dataNum = 1 << logDataNum;
    for(int i = 0; i < n; i++) {
        if(i % dataNum == 0) {
            std::cout << "<<";
        }
        std::cout << i << " : " << a1[i].real() << " // " << a2[i].real();
        if(i % dataNum == 0) {
            std::cout << ">>";
        }
        std:: cout << std::endl;
    }
}


void decAndPrint(std::string str, Ciphertext& cipher, Scheme& scheme, SecretKey& secretKey) {
    if (DEC_AND_PRINT) {
        complex<double>* dvec = scheme.decrypt(secretKey, cipher);
        cout << "==== " << str << " ====" << endl;

        long POWER_OF_TEN = 1;
        while(POWER_OF_TEN * 100 < cipher.n){
            POWER_OF_TEN *= 10;
        }

        for(int i = 0; i < cipher.n; i++) {
            if (cipher.n < 100 || i % POWER_OF_TEN == 0) {
                std::cout << i << " : " << dvec[i] << std::endl;
            }        
        }
    }    
}

void decAndPrintTwo(std::string str, Ciphertext& cipher1, Ciphertext& cipher2, Scheme& scheme, SecretKey& secretKey){
    if (DEC_AND_PRINT) {
            /* code */    
        complex<double>* dvec1 = scheme.decrypt(secretKey, cipher1);
        complex<double>* dvec2 = scheme.decrypt(secretKey, cipher2);
        
        cout << "==== " << str << " ====" << endl;

        long POWER_OF_TEN = 1;
        while(POWER_OF_TEN * 100 < cipher1.n){
            POWER_OF_TEN *= 10;
        }

        for(int i = 0; i < cipher1.n; i++) {
            if (cipher1.n < 100 || i % POWER_OF_TEN == 0) {
                cout << i << " : " << dvec1[i].real() << ", " << dvec2[i].real() << endl;
            }        
        }
    }
}

void PrintUtils::nprint(std::string str, bool isPrint) {
    if(isPrint) std::cout << str << std::endl;
}