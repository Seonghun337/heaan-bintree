#include "TestSort.h"

void TestSort::sort(Parameter param, long iter, bool increase) {
    srand(time(NULL));
	SetNumThreads(16);
    TimeUtils timeutils;
	PrintUtils::parameter(param, "TestSort");
    long n = 1 << param.log2n;
    long logp = param.logp;

    timeutils.start("TestSort KeyGen");
    Ring ring(param.logN, param.logQ);
    SecretKey secretKey(ring);
    BootScheme scheme(secretKey, ring);
    scheme.addConjKey(secretKey);
    scheme.addLeftRotKeys(secretKey);
    scheme.addRightRotKeys(secretKey);
    timeutils.stop("TestSort KeyGen");

	timeutils.start("Bootstrapping Helper construct");
	BootHelper bootHelper(param.log2n, param.radix, param.logc, scheme, ring, secretKey);
	timeutils.stop("Bootstrapping Helper construct");

    // double* mvec = EvaluatorUtils::randomRealArray(n);
    double* mvec = new double[n];
    for (int i = 0; i < n; i++) {
        mvec[i] = 1. / n * (double) i;
    }
    random_shuffle(&mvec[0], &mvec[n]);
       
	Ciphertext cipher = scheme.encrypt(mvec, n, param.logp, param.logQ);

    timeutils.start("EncSort");
    EncSorting encSorting(param, iter);
    // encSorting.runEncSorting(cipher, scheme, ring, bootHelper, increase);
    encSorting.runEncSortingDec(cipher, scheme, ring, bootHelper, increase, secretKey);
    timeutils.stop("EncSort"); 

    // run PlainSort
    CyclicArray ca(mvec, 1 << param.log2n);
    PlainSort plainSort;
    plainSort.runPlainSorting(ca, param.log2n, increase);
    mvec = ca.getArray();

    // Print Result and Difference //	
	complex<double>* dvec = scheme.decrypt(secretKey, cipher);
    PrintUtils::printArrays(mvec, dvec, n);
    PrintUtils::averageDifference(mvec, dvec, n);

}

void TestSort::tableSort(Parameter param, long logDataNum, long colNum, long invIter, long compIter, bool increase) {
    srand(time(NULL));
	SetNumThreads(16);
    TimeUtils timeutils;
    
	PrintUtils::parameter(param, "TestTableSort");
    long n = 1 << param.log2n;
    long logp = param.logp;

    timeutils.start("TestTableSort KeyGen");
    Ring ring(param.logN, param.logQ);
    SecretKey secretKey(ring);
    BootScheme scheme(secretKey, ring);
    scheme.addConjKey(secretKey);
    scheme.addLeftRotKeys(secretKey);
    scheme.addRightRotKeys(secretKey);
    timeutils.stop("TestTableSort KeyGen");

	timeutils.start("Bootstrapping Helper construct");
	BootHelper bootHelper(param.log2n, param.radix, param.logc, scheme, ring, secretKey);
	timeutils.stop("Bootstrapping Helper construct");

    double* mvec = EvaluatorUtils::randomRealArray(n);
    
	Ciphertext cipher = scheme.encrypt(mvec, n, param.logp, param.logQ);

    timeutils.start("EncTableSort");
    EncSorting encSorting(param, invIter, compIter);
    encSorting.runEncTableSorting(cipher, logDataNum, colNum, scheme, ring, bootHelper, secretKey, increase);
    timeutils.stop("EncTableSort"); 

    // run PlainSort
    CyclicArray ca(mvec, 1 << param.log2n);
    PlainSort plainSort;
    plainSort.runPlainTableSorting(ca, param.log2n, logDataNum, colNum, increase);
    mvec = ca.getArray();
    
    // Print Result and Difference //	
	complex<double>* dvec = scheme.decrypt(secretKey, cipher);
    PrintUtils::printArraysWithDataNum(mvec, dvec, n, logDataNum, colNum);
    PrintUtils::averageDifference(mvec, dvec, n);    
}

void TestSort:: merge(Parameter param, long iter, long logNum) {
    srand(time(NULL));
	SetNumThreads(16);
    TimeUtils timeutils;
    long num = 1 << logNum;
    
	PrintUtils::parameter(param, "Merge");
    long n = 1 << param.log2n;
    long logp = param.logp;

    timeutils.start("KeyGen");
    Ring ring(param.logN, param.logQ);
    SecretKey secretKey(ring);
    BootScheme scheme(secretKey, ring);
    scheme.addConjKey(secretKey);
    scheme.addLeftRotKeys(secretKey);
    scheme.addRightRotKeys(secretKey);
    timeutils.stop("KeyGen");

	timeutils.start("Bootstrapping Helper construct");
	BootHelper bootHelper(param.log2n, param.radix, param.logc, scheme, ring, secretKey);
	timeutils.stop("Bootstrapping Helper construct");

    double ** mvec = new double*[num];
    Ciphertext* cipher = new Ciphertext[num];
    PlainSort plainSort;
    CyclicArray* ca = new CyclicArray[num];
    for(int i = 0; i < num; i++) {
        mvec[i] = EvaluatorUtils::randomRealArray(n);
        ca[i] = CyclicArray(mvec[i], n);
        plainSort.runPlainSorting(ca[i], param.log2n, i % 2 == 0);
        cipher[i] = scheme.encrypt(ca[i].getArray(), n, param.logp, param.logQ);
    }

    EncSorting encSorting(param, iter);
    timeutils.start("Bitonic Merge");
    encSorting.bitonicMerge(cipher, logNum, scheme, ring, bootHelper);
    timeutils.stop("Bitonic Merge"); 

    //* decrypt
    complex<double>** dvec = new complex<double>*[num];
    for(int i = 0; i < num; i++) {
         dvec[i] = scheme.decrypt(secretKey, cipher[i]);
    } 

    //* run plain bitonicMerge
    plainSort.bitonicMerge(ca, param.log2n, logNum);

    //* Print merged arrays
    // for (int i = 0; i < num; i++) {
    //     PrintUtils::printArrays(ca[i].getArray(), dvec[i], n);
    // }

    //* Print log2(avg of errors)
    for (int i = 0; i < num; i++) {
        PrintUtils::averageDifference(ca[i].getArray(), dvec[i], n);
    }    
}

void TestSort:: tableMerge(Parameter param, long logNum, long logDataNum, long colNum, long invIter, long compIter) {
    srand(time(NULL));
	SetNumThreads(16);
    TimeUtils timeutils;
    long num = 1 << logNum;
    
	PrintUtils::parameter(param, "tableMerge");
    long n = 1 << param.log2n;
    long logp = param.logp;

    timeutils.start("KeyGen");
    Ring ring(param.logN, param.logQ);
    SecretKey secretKey(ring);
    BootScheme scheme(secretKey, ring);
    scheme.addConjKey(secretKey);
    scheme.addLeftRotKeys(secretKey);
    scheme.addRightRotKeys(secretKey);
    timeutils.stop("KeyGen");

	timeutils.start("Bootstrapping Helper construct");
	BootHelper bootHelper(param.log2n, param.radix, param.logc, scheme, ring, secretKey);
	timeutils.stop("Bootstrapping Helper construct");

    double ** mvec = new double*[num];
    Ciphertext* cipher = new Ciphertext[num];
    PlainSort plainSort;
    CyclicArray* ca = new CyclicArray[num];
    for(int i = 0; i < num; i++) {
        mvec[i] = EvaluatorUtils::randomRealArray(n); 
        ca[i] = CyclicArray(mvec[i], n);
        plainSort.runPlainTableSorting(ca[i], param.log2n, logDataNum, colNum, i % 2 == 0);
        cipher[i] = scheme.encrypt(ca[i].getArray(), n, param.logp, param.logQ);
    }

    for(int i = 0; i < num; i++) {
        cout << "ca[" << i << "] = ";
        ca[i].printAsVector();
    }
    

    EncSorting encSorting(param, invIter, compIter);
    timeutils.start("Bitonic Table Merge");
    encSorting.bitonicTableMerge(cipher, logNum, logDataNum, colNum, scheme, ring, bootHelper, secretKey);
    timeutils.stop("Bitonic Table Merge"); 

    //* decrypt
    complex<double>** dvec = new complex<double>*[num];
    for(int i = 0; i < num; i++) {
         dvec[i] = scheme.decrypt(secretKey, cipher[i]);
    } 

    //* run plain bitonicMerge
    plainSort.bitonicTableMerge(ca, param.log2n, logNum, logDataNum, colNum);

    //* Print merged arrays
    for (int i = 0; i < num; i++) {
        PrintUtils::printArraysWithDataNum(ca[i].getArray(), dvec[i], n, logDataNum, colNum);
    }

    //* Print log2(avg of errors)
    for (int i = 0; i < num; i++) {
        PrintUtils::averageDifference(ca[i].getArray(), dvec[i], n);
    }    
}

void TestSort::sortAndMerge(Parameter param, long iter, long logNum) {
    srand(time(NULL));
	SetNumThreads(16);
    TimeUtils timeutils;
    
	PrintUtils::parameter(param, "sortAndMerge");
    long n = 1 << param.log2n;
    long logp = param.logp;
    long num = 1 << logNum;

    timeutils.start("KeyGen");
    Ring ring(param.logN, param.logQ);
    SecretKey secretKey(ring);
    BootScheme scheme(secretKey, ring);
    scheme.addConjKey(secretKey);
    scheme.addLeftRotKeys(secretKey);
    scheme.addRightRotKeys(secretKey);
    timeutils.stop("KeyGen");

	timeutils.start("Bootstrapping Helper construct");
	BootHelper bootHelper(param.log2n, param.radix, param.logc, scheme, ring, secretKey);
	timeutils.stop("Bootstrapping Helper construct");

    double** mvec = new double*[num];
    Ciphertext* cipher = new Ciphertext[num];
    for(int i = 0; i < num; i++) {
        mvec[i] = EvaluatorUtils::randomRealArray(n);
        cipher[i] = scheme.encrypt(mvec[i], n, param.logp, param.logQ);
    }

    EncSorting encSorting(param, iter);
    
    TimeUtils timeTotal;
    
    timeTotal.start("Sort And Merge");

    timeutils.start("EncSort");
    for (int i = 0; i < num; i++) {
        encSorting.runEncSorting(cipher[i], scheme, ring, bootHelper);
    }
    timeutils.stop("EncSort");

    timeutils.start("Reverse");
    encSorting.reverseHalf(cipher, logNum, scheme, ring, bootHelper);
    timeutils.stop("Reverse");

    timeutils.start("Bitonic Merge");
    encSorting.bitonicMerge(cipher, logNum, scheme, ring, bootHelper);
    timeutils.stop("Bitonic Merge"); 

    timeTotal.stop("Total Sort And Merge");



    //* run plain bitonicMerge
    PlainSort plainSort;
    CyclicArray* ca = new CyclicArray[num];
    for (int i = 0; i < num; i++) {
        ca[i] = CyclicArray(mvec[i], n);
        plainSort.runPlainSorting(ca[i], param.log2n, i % 2 == 0);
    }
    plainSort.bitonicMerge(ca, param.log2n, logNum);

    //* decrypt
    complex<double>** dvec = new complex<double>*[num];
    for(int i = 0; i < num; i++) {
         dvec[i] = scheme.decrypt(secretKey, cipher[i]);
    } 

    //* Print full arrays
    for (int i = 0; i < num; i++) {
        PrintUtils::printArrays(ca[i].getArray(), dvec[i], n);
        cout << "===" << endl;
    }

    //* Print log2(avg of errors)
    for (int i = 0; i < num; i++) {
        PrintUtils::averageDifference(ca[i].getArray(), dvec[i], n);
    }    
}

void TestSort::bitonicSort(Parameter param, long iter, bool increase) {
    srand(time(NULL));
	SetNumThreads(16);
    TimeUtils timeutils;
	PrintUtils::parameter(param, "TestBitonicSort");
    long n = 1 << param.log2n;
    long logp = param.logp;

    timeutils.start("TestBitonicSort KeyGen");
    Ring ring(param.logN, param.logQ);
    SecretKey secretKey(ring);
    BootScheme scheme(secretKey, ring);
    scheme.addConjKey(secretKey);
    scheme.addLeftRotKeys(secretKey);
    scheme.addRightRotKeys(secretKey);
    timeutils.stop("TestBitonicSort KeyGen");

	timeutils.start("Bootstrapping Helper construct");
	BootHelper bootHelper(param.log2n, param.radix, param.logc, scheme, ring, secretKey);
	timeutils.stop("Bootstrapping Helper construct");

    // double* mvec = EvaluatorUtils::randomRealArray(n);
    double* mvec = new double[n];
    for (int i = 0; i < n; i++) {
        mvec[i] = 1. / n * (double) i;
    }
    random_shuffle(&mvec[0], &mvec[n]);
       
	Ciphertext cipher = scheme.encrypt(mvec, n, param.logp, param.logQ);

    timeutils.start("EncSort");
    EncSorting encSorting(param, iter);
    encSorting.runBitonicSortDec(cipher, scheme, ring, bootHelper, increase, secretKey);
    timeutils.stop("EncSort"); 

    // run PlainSort
    CyclicArray ca(mvec, 1 << param.log2n);
    PlainSort plainSort;
    plainSort.runPlainSorting(ca, param.log2n, increase);
    mvec = ca.getArray();

    // Print Result and Difference //	
	complex<double>* dvec = scheme.decrypt(secretKey, cipher);
    PrintUtils::printArrays(mvec, dvec, n);
    PrintUtils::averageDifference(mvec, dvec, n);
}