#include "TestBoot.h"

void TestBoot::bootstrapping(Parameter parameter) {
	PrintUtils::parameter(parameter, "Improved Bootstrapping");
	
	long n = 1 << parameter.log2n;   
	
	TimeUtils timeutils;
	timeutils.start("KeyGen");
	Ring ring(parameter.logN, parameter.logQ);
	SecretKey secretKey(ring);
	BootScheme scheme(secretKey, ring);
	scheme.addConjKey(secretKey);
	scheme.addLeftRotKeys(secretKey);
	
	srand(time(NULL));
	SetNumThreads(16);
	timeutils.stop("KeyGen");

	timeutils.start("Bootstrapping Helper construct");
	BootHelper boothelper(parameter.log2n, parameter.radix, parameter.logc, scheme, ring, secretKey);
	timeutils.stop("Bootstrapping Helper construct");

	complex<double>* r10 = EvaluatorUtils::randomComplexArray(n);
    // for (int i = 0; i < n; i++) {
    //     r10[i] *= -1;
    // }
    
	Ciphertext cipher = scheme.encrypt(r10, n, parameter.logp, parameter.logQ);
    double* one = new double[n];
    for (int i = 0; i < n; i++) {
        one[i] = 1.;
    }
    ZZ* onePoly = new ZZ[1 << parameter.logN];
    ring.encode(onePoly, one, n, parameter.logp);
    while(cipher.logq > parameter.logq + parameter.logp) {
        cout << "mult" << endl;

        scheme.multByPolyAndEqual(cipher, onePoly, parameter.logp);
        scheme.reScaleByAndEqual(cipher, parameter.logp);
    }
	timeutils.start("Improved bootstrapping");
	complex<double>* dvec1 = scheme.decrypt(secretKey, cipher);
	boothelper.bootstrapping_cos(cipher, parameter.logq, parameter.logQ, 5);
    complex<double>* dvec2 = scheme.decrypt(secretKey, cipher);
	timeutils.stop("Improved bootstrapping");

	cout << "* Before logQ = " << parameter.logq << endl;
	cout << "* After logQ = " << cipher.logq << endl;

	// Print Result and Difference //
    complex<double>* dvec = scheme.decrypt(secretKey, cipher);
    // PrintUtils::printArrays(r10, dvec2, n);
    PrintUtils::averageDifference(r10, dvec2, n);
    // PrintUtils::printArrays(dvec1, dvec2, n);
    PrintUtils::averageDifference(dvec1, dvec2, n);
	
	return;
}

void TestBoot::approxSqrt(Parameter parameter, long iter) {
	long n = 1 << parameter.log2n;

	PrintUtils::parameter(parameter, "TestBoot::approxSqrt");
	
	TimeUtils timeutils;
	timeutils.start("KeyGen");
	Ring ring(parameter.logN, parameter.logQ);
	SecretKey secretKey(ring);
	BootScheme scheme(secretKey, ring);
	scheme.addConjKey(secretKey);
	scheme.addLeftRotKeys(secretKey);
	timeutils.stop("KeyGen");

	timeutils.start("Bootstrapping Helper construct");
	BootHelper bootHelper(parameter.log2n, parameter.radix, parameter.logc, scheme, ring, secretKey);
	timeutils.stop("Bootstrapping Helper construct");

	double* mvec = EvaluatorUtils::randomRealArray(n);
    for(int i = 0; i < n; i++) {
        mvec[i] = -0.00000001;
    }
    // double* mvec = new double[n];
    // mvec[0] = 0.0086326; // 0.00821181
    // mvec[1] = 0.1; // 0.0999609
    // mvec[2] = 0.00555455; // 0.0049605
    // mvec[3] = 0.1; // 0.0999609
    // mvec[4] = 0.0155735; // 0.015328
    // mvec[5] = 0.1; // 0.0999609
    // mvec[6] = 0.061951; // 0.061888
    // mvec[7] = 0.1; // 0.0999609
    // mvec[8] = 0.0482967; // 0.0482158
    // mvec[9] = 0.1; // 0.0999609
    // mvec[10] = 0.0374798; // 0.0373758
    // mvec[11] = 0.1; // 0.0999609
    // mvec[12] = 0.0775831; // 0.0775327
    // mvec[13] = 0.000242687; // 0.0999609
    // mvec[14] = 0.0109508; // 0.0106098
    // mvec[15] = 0.443224; // 0.443215
    
    // for(int i = 0; i < n; i++) {
    //     mvec[i] = mvec[i] * mvec[i];
    //     // cout << "mvec[" << i << "] = " << mvec[i] << endl;
    // }
	// Ciphertext cipher = scheme.encrypt(mvec, n, parameter.logp, parameter.logQ);
    Ciphertext cipher2 = scheme.encrypt(mvec, n, parameter.logp, parameter.logQ);
    
    // for (int i = 0; i < 3; i++) {
    //     for(int j = 0; j < n; j++) {
    //         mvec[j] = mvec[j] * mvec[j];
    //     }   
    //     scheme.squareAndEqual(cipher2);
    //     scheme.reScaleByAndEqual(cipher2, parameter.logp);
    // }
    // for (int i = 0; i < 10; i++) {
    //     for(int j = 0; j < n; j++) {
    //         mvec[j] = mvec[j] * 1.1;
    //     }   
    //     scheme.multByConstAndEqual(cipher2, 1.1, parameter.logp);
    //     scheme.reScaleByAndEqual(cipher2, parameter.logp);
    // }
    // scheme.decryptAndPrint("cipher2", secretKey, cipher2);

    // scheme.modDownToAndEqual(cipher, parameter.logq);
    // scheme.modDownToAndEqual(cipher2, parameter.logq);
    // scheme.modDownToAndEqual(cipher3, parameter.logq);   
    
    BootAlgo bootAlgo(parameter, 5, 5, 0, true);
    // timeutils.start("Sqrt");
	// bootAlgo.approxSqrt(cipher2, scheme, bootHelper);
    // timeutils.stop("Sqrt");
    timeutils.start("Sqrt2");
    // double addCnst = 0.5;
    // scheme.addConstAndEqual(cipher2, addCnst, parameter.logp);
    bootAlgo.approxSqrt2Dec(cipher2, scheme, bootHelper, secretKey);
    // bootAlgo.approxSqrt4Dec(cipher2, scheme, bootHelper, secretKey);

    timeutils.stop("Sqrt2");
    // timeutils.start("Sqrt3");
    // bootAlgo.approxSqrt3(cipher3, scheme, bootHelper);
    // timeutils.stop("Sqrt3");
    

    // Print Result and Difference //	
    // cout << "1 : comsumed logQ = " << parameter.logQ - cipher.logq << endl;   
	// complex<double>* dvec = scheme.decrypt(secretKey, cipher);
    // cout << "2 : comsumed logQ = " << parameter.logQ - cipher2.logq << endl;
    complex<double>* dvec2 = scheme.decrypt(secretKey, cipher2);
    // cout << "3 : comsumed logQ = " << parameter.logQ - cipher3.logq << endl;
    // complex<double>* dvec3 = scheme.decrypt(secretKey, cipher3);
    for(int i = 0; i < n; i++) {
        mvec[i] = sqrt(mvec[i]);
        // mvec[i] = sqrt(mvec[i]);
    }
    // PrintUtils::printArrays(mvec, dvec, n);
    // PrintUtils::averageDifference(mvec, dvec, n);
    PrintUtils::printArrays(mvec, dvec2, n);
    PrintUtils::averageDifference(mvec, dvec2, n);
    // PrintUtils::printArrays(mvec, dvec3, n);
    // PrintUtils::averageDifference(mvec, dvec3, n);
}

void TestBoot::approxInverse(Parameter parameter, long iter) {
	long n = 1 << parameter.log2n;

	PrintUtils::parameter(parameter, "TestBoot::approxInverse");
	
	TimeUtils timeutils;
	timeutils.start("KeyGen");
	Ring ring(parameter.logN, parameter.logQ);
	SecretKey secretKey(ring);
	BootScheme scheme(secretKey, ring);
	scheme.addConjKey(secretKey);
	scheme.addLeftRotKeys(secretKey);
	timeutils.stop("KeyGen");

	timeutils.start("Bootstrapping Helper construct");
	BootHelper bootHelper(parameter.log2n, parameter.radix, parameter.logc, scheme, ring, secretKey);
	timeutils.stop("Bootstrapping Helper construct");

	double* mvec = EvaluatorUtils::randomRealArray(n);
    for (int i = 0; i < n; i++) {
        mvec[i] += 0.5;
    }
    
    
	Ciphertext cipher = scheme.encrypt(mvec, n, parameter.logp, parameter.logQ);

    
    timeutils.start("approxInverse");
    BootAlgo bootAlgo(parameter, iter);
    long start = cipher.logq;
    // bootHelper.bootstrapping(cipher, parameter.logq, parameter.logQ, parameter.logT);
    bootHelper.bootstrapping_cos(cipher, parameter.logq, parameter.logQ, 5);
	bootAlgo.approxInverse(cipher, scheme, bootHelper);
    // bootAlgo.approxInverseWithDec(cipher, scheme, bootHelper, secretKey);
    timeutils.stop("approxInverse");
    cout << "consumed logq = " << start - cipher.logq << endl;
    // Print Result and Difference //	
	complex<double>* dvec = scheme.decrypt(secretKey, cipher);
    double* invvec = new double[n];
    for(int i = 0; i < n; i++) {
        invvec[i] = 1 / mvec[i];
    }
    cout << "mvec // invvec // dvec" << endl;
    for (int i = 0; i < n; i++)
    {
        cout << i << " : " << mvec[i] << " // " << invvec[i] << " // " << dvec[i].real() << endl;
    }
    
    PrintUtils::averageDifference(invvec, dvec, n);
}

void TestBoot::approxComp(Parameter parameter, long invIter, long compIter) {
    long n = 1 << parameter.log2n;

	PrintUtils::parameter(parameter, "TestBoot::approxComp");
	
	TimeUtils timeutils;
	timeutils.start("KeyGen");
	Ring ring(parameter.logN, parameter.logQ);
	SecretKey secretKey(ring);
	BootScheme scheme(secretKey, ring);
	scheme.addConjKey(secretKey);
	scheme.addLeftRotKeys(secretKey);
	timeutils.stop("KeyGen");

	timeutils.start("Bootstrapping Helper construct");
	BootHelper bootHelper(parameter.log2n, parameter.radix, parameter.logc, scheme, ring, secretKey);
	timeutils.stop("Bootstrapping Helper construct");


	double* mvec1 = EvaluatorUtils::randomRealArray(n);
    double* mvec2 = EvaluatorUtils::randomRealArray(n);

	Ciphertext cipher1 = scheme.encrypt(mvec1, n, parameter.logp, parameter.logQ);
    Ciphertext cipher2 = scheme.encrypt(mvec2, n, parameter.logp, parameter.logQ);

    
    timeutils.start("approxComp");
    BootAlgo bootAlgo(parameter, invIter, compIter);
    // bootHelper.bootstrapping(cipher1, parameter.logq, parameter.logQ, parameter.logT);
    // bootHelper.bootstrapping(cipher2, parameter.logq, parameter.logQ, parameter.logT);
	bootAlgo.comparison(cipher1, cipher2, scheme, bootHelper);
    timeutils.stop("approxComp");

    // Print Result and Difference //	
	complex<double>* dvec = scheme.decrypt(secretKey, cipher1);
    complex<double>* dvec2 = scheme.decrypt(secretKey, cipher2);
    double* compvec = new double[n];    
    cout << "mvec1 // mvec2 // compvec // dvec1 // dvec2" << endl;
    for (int i = 0; i < n; i++) {
        if (mvec1[i] >= mvec2[i]) {
            compvec[i] = 1.;
        } else {
            compvec[i] = 0.;
        }
        cout << i << " : " << mvec1[i] << " // " << mvec2[i] << " // " << compvec[i] << " // "  << dvec[i].real() << " // " << dvec2[i].real() << endl;
    }
    
    PrintUtils::averageDifference(compvec, dvec, n);   
}

void TestBoot::minMax(Parameter param, long iter) {
    PrintUtils::parameter(param, "TestBoot::minMax");
	long n = 1 << param.log2n;

    TimeUtils timeutils;
    timeutils.start("KeyGen");
    Ring ring(param.logN, param.logQ);
    SecretKey secretKey(ring);
    BootScheme scheme(secretKey, ring);
    scheme.addConjKey(secretKey);
    scheme.addLeftRotKeys(secretKey);
	srand(time(NULL));
	SetNumThreads(16);
    timeutils.stop("KeyGen");

	timeutils.start("Bootstrapping Helper construct");
	BootHelper boothelper(param.log2n, param.radix, param.logc, scheme, ring, secretKey);
	timeutils.stop("Bootstrapping Helper construct");

	double* mvec1 = EvaluatorUtils::randomRealArray(n);
    double* mvec2 = EvaluatorUtils::randomRealArray(n);
    
	Ciphertext cipher1 = scheme.encrypt(mvec1, n, param.logp, param.logQ);
    Ciphertext cipher2 = scheme.encrypt(mvec2, n, param.logp, param.logQ);

    scheme.modDownToAndEqual(cipher1, param.logq);
    scheme.modDownToAndEqual(cipher2, param.logq);
    
	timeutils.start("minMax");
    BootAlgo bootAlgo(param, iter);
    // bootAlgo.minMax(cipher1, cipher2, scheme, boothelper);
    bootAlgo.minMaxDec(cipher1, cipher2, scheme, boothelper, secretKey);
    timeutils.stop("minMax");

	complex<double>* dmax = scheme.decrypt(secretKey, cipher2);
    complex<double>* dmin = scheme.decrypt(secretKey, cipher1);
    
    double* max = new double[n];
    for(int i = 0; i < n; i++) {
        if(mvec1[i] > mvec2[i]) {
            max[i] = mvec1[i];
        } else {
            max[i] = mvec2[i];
        }   
    }
    PrintUtils::averageDifference(max, dmax, n);
}

void TestBoot::compAndSwap(Parameter param, long iter) {
    srand(time(NULL));
	SetNumThreads(16);
    
    long n = 1 << param.log2n;
	
    PrintUtils::parameter(param, "TestBoot::compAndSwap");

    TimeUtils timeutils;
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


    double* mvec = EvaluatorUtils::randomRealArray(n);

	// Ciphertext cipher = scheme.encrypt(mvec, n, param.logp, param.logQ);
    Ciphertext cipher = scheme.encrypt(mvec, n, param.logp, param.logq);

    MaskingGenerator mg(param.log2n);
    double** mask = mg.getMasking();

    PrintUtils::printSingleArray("mask[0]", mask[0], n);

    timeutils.start("CompAndSwap");
    BootAlgo bootAlgo(param, iter);
    // bootAlgo.compAndSwapDec(cipher, mask[0], 1, scheme, ring, bootHelper, 1, secretKey);
    bootAlgo.compAndSwap(cipher, mask, 0, 1, scheme, ring, bootHelper);
	
    timeutils.stop("CompAndSwap");

    for(int i = 0; i < n / 2; i++) {
        if(mvec[2 * i] > mvec[2 * i + 1]) {
            double x = mvec[2 * i];
            mvec[2 * i] = mvec[2 * i + 1];
            mvec[2 * i + 1] = x;
        }
    }

	// MaskingGenerator mg(param.log2n);
    // double** maskIncrease = mg.getBitonicMergeMasking();
	// BootAlgo bootAlgo(param, iter);
	// bootAlgo.compAndSwap(cipher, maskIncrease[0], 1 << (param.log2n - 1), scheme, ring, bootHelper);
	// for(int i = 0; i < n / 2; i++) {
    //     if(mvec[i] > mvec[i + n / 2]) {
    //         double x = mvec[i];
    //         mvec[i] = mvec[i + n / 2];
    //         mvec[i + n / 2] = x;
    //     }
    // }


	complex<double>* dvec = scheme.decrypt(secretKey, cipher);
	PrintUtils::printArrays(mvec, dvec, n);
    PrintUtils::averageDifference(mvec, dvec, n);
}

void TestBoot::reverse(Parameter param) {
    srand(time(NULL));
	SetNumThreads(16);
    
    long n = 1 << param.log2n;
	
    PrintUtils::parameter(param, "TestBoot::reverse");

    TimeUtils timeutils;
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


    double* mvec = EvaluatorUtils::randomRealArray(n);

	Ciphertext cipher = scheme.encrypt(mvec, n, param.logp, param.logQ);

	MaskingGenerator mg(param.log2n);
    double** mask = mg.getBitonicMergeMasking();
	BootAlgo bootAlgo(param, 0);
	bootAlgo.reverse(cipher, mask, scheme, ring, bootHelper);

	complex<double>* dvec = scheme.decrypt(secretKey, cipher);
	PrintUtils::printArrays(mvec, dvec, n);
}

void TestBoot::compAndSwapTable(Parameter parameter, long logDataNum, long colNum, long invIter, long compIter) {
    long n = 1 << parameter.log2n;

	PrintUtils::parameter(parameter, "TestBoot::compAndSwapTable");
	
	TimeUtils timeutils;
	timeutils.start("KeyGen");
	Ring ring(parameter.logN, parameter.logQ);
	SecretKey secretKey(ring);
	BootScheme scheme(secretKey, ring);
	scheme.addConjKey(secretKey);
	scheme.addLeftRotKeys(secretKey);
    scheme.addRightRotKeys(secretKey);
	timeutils.stop("KeyGen");

	timeutils.start("Bootstrapping Helper construct");
	BootHelper bootHelper(parameter.log2n, parameter.radix, parameter.logc, scheme, ring, secretKey);
	timeutils.stop("Bootstrapping Helper construct");

	double* mvec = EvaluatorUtils::randomRealArray(n);
    // for (int i = 0; i < n; i++) {
    //     mvec[i] += 0.5;
    // }
    
	Ciphertext cipher = scheme.encrypt(mvec, n, parameter.logp, parameter.logQ);

    MaskingGenerator mg(parameter.log2n, logDataNum);
    double** mask = mg.getMasking();
    MaskingGenerator mg2(parameter.log2n, logDataNum);
    double** maskOther = mg2.getMaskingOther();
    MaskingGenerator mgTable(parameter.log2n, logDataNum, colNum, true);
    double** maskTable = mgTable.getMasking();
    MaskingGenerator mgTable2(parameter.log2n, logDataNum, colNum, true);
    double** maskTableOther = mgTable2.getMaskingOther();
    
    long logn = parameter.log2n - logDataNum;
    long maskNum = logn * (logn + 1) / 2;
    // PrintUtils::printSingleMatrix("mask", mask, maskNum, n);
    // cout << endl;
    // PrintUtils::printSingleMatrix("maskOther", maskOther, maskNum, n);
    // cout << endl;
    // PrintUtils::printSingleMatrix("maskTable", maskTable, maskNum, n);
    // cout << endl;
    // PrintUtils::printSingleMatrix("maskTableOther", maskTableOther, maskNum, n);
    
    complex<double>* dvec;

    timeutils.start("compAndSwapTable");
    BootAlgo bootAlgo(parameter, invIter, compIter);
    // bootHelper.bootstrapping(cipher, parameter.logq, parameter.logQ, parameter.logT);
	bootAlgo.compAndSwapTable(cipher, logDataNum, colNum, mask[0], maskOther[0], maskTable[0], maskTableOther[0], 1 << logDataNum, scheme, ring, bootHelper, secretKey);
    dvec = scheme.decrypt(secretKey, cipher);
    // PrintUtils::printArrays(mvec, dvec, n);
    PrintUtils::averageDifference(mvec, dvec, n);
    bootAlgo.compAndSwapTable(cipher, logDataNum, colNum, mask[1], maskOther[1], maskTable[1], maskTableOther[1], 1 << (logDataNum + 1), scheme, ring, bootHelper, secretKey);
    dvec = scheme.decrypt(secretKey, cipher);
    // PrintUtils::printArrays(mvec, dvec, n);
    PrintUtils::averageDifference(mvec, dvec, n);
    bootAlgo.compAndSwapTable(cipher, logDataNum, colNum, mask[2], maskOther[2], maskTable[2], maskTableOther[2], 1 << (logDataNum), scheme, ring, bootHelper, secretKey);
    dvec = scheme.decrypt(secretKey, cipher);
    // PrintUtils::printArrays(mvec, dvec, n);
    PrintUtils::averageDifference(mvec, dvec, n);
    timeutils.stop("compAndSwapTable");
    // Print Result and Difference //	
	// dvec = scheme.decrypt(secretKey, cipher);
    // PrintUtils::printArrays(mvec, dvec, n);
    // PrintUtils::averageDifference(mvec, dvec, n);
}

void TestBoot::halfCleaner(Parameter param, long iter) {
    srand(time(NULL));
	SetNumThreads(16);
    
    long n = 1 << param.log2n;
	
    PrintUtils::parameter(param, "TestBoot::halfCleaner");

    TimeUtils timeutils;
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


    double* mvec = EvaluatorUtils::randomRealArray(n);

	Ciphertext cipher = scheme.encrypt(mvec, n, param.logp, param.logQ);

	MaskingGenerator mg(param.log2n);
    double** mask = mg.getBitonicMergeMasking();
	BootAlgo bootAlgo(param, iter);
	bootAlgo.halfCleaner(cipher, mask[0], 1 << (param.log2n - 1), scheme, ring, bootHelper, secretKey);

	complex<double>* dvec = scheme.decrypt(secretKey, cipher);
	PrintUtils::printArrays(mvec, dvec, n);
}