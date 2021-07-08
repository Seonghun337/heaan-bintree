
#include "TestEnc.h"

void TestEnc::approxSqrt(Parameter param, long iter) {
    srand(time(NULL));
	SetNumThreads(16);
    
    TimeUtils timeutils;
    timeutils.start("TestEnc KeyGen");
    Ring ring(param.logN, param.logQ);
    SecretKey secretKey(ring);
    Scheme scheme(secretKey, ring);
    scheme.addConjKey(secretKey);
    scheme.addLeftRotKeys(secretKey);
    timeutils.stop("TestEnc KeyGen");

	PrintUtils::parameter(param, "TestEnc::sqrt");
    long n = 1 << param.log2n;
    long logp = param.logp;

    double* mvec = EvaluatorUtils::randomRealArray(n);
    
	Ciphertext cipher = scheme.encrypt(mvec, n, param.logp, param.logQ);

    Ciphertext sqrtCipher;
    timeutils.start("Sqrt");
    EncAlgorithm encAlgo;
    encAlgo.approxSqrt(sqrtCipher, cipher, iter, param, scheme);
    timeutils.stop("Sqrt");

    // Print Result and Difference //	
	complex<double>* dvec = scheme.decrypt(secretKey, sqrtCipher);
    for(int i = 0; i < n; i++) {
        mvec[i] = sqrt(mvec[i]);
    }
    PrintUtils::averageDifference(mvec, dvec, n);
    
}

void TestEnc::minMax(Parameter param, long iter) {

    srand(time(NULL));
	SetNumThreads(16);

    PrintUtils::parameter(param, "TestEnc::minMax");

    TimeUtils timeutils;
    timeutils.start("KeyGen");
    Ring ring(param.logN, param.logQ);
    SecretKey secretKey(ring);
    Scheme scheme(secretKey, ring);
    scheme.addConjKey(secretKey);
    scheme.addLeftRotKeys(secretKey);
    timeutils.stop("KeyGen");

    long n = 1 << param.log2n;

    double* mvec1 = EvaluatorUtils::randomRealArray(n);
    double* mvec2 = EvaluatorUtils::randomRealArray(n);
    
	Ciphertext cipher1 = scheme.encrypt(mvec1, n, param.logp, param.logQ);
    Ciphertext cipher2 = scheme.encrypt(mvec2, n, param.logp, param.logQ);
    
    Ciphertext minCipher, maxCipher;
    EncAlgorithm encAlgo;
    timeutils.start("minMax");
    encAlgo.minMax(minCipher, maxCipher, cipher1, cipher2, iter, param, scheme);
    timeutils.stop("minMax");
    
    
    complex<double>* dmax = scheme.decrypt(secretKey, maxCipher);
    complex<double>* dmin = scheme.decrypt(secretKey, minCipher);
    
    for(int i = 0; i < n; i++) {
        cout << i << " : " << 
            mvec1[i] << ", " << mvec2[i] << " /// -> /// " <<
            dmax[i].real() << ", " << dmin[i].real() << endl;
    }

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

void TestEnc::compAndSwap(Parameter param, long iter) {
    srand(time(NULL));
	SetNumThreads(16);
    
    long n = 1 << param.log2n;
	
    PrintUtils::parameter(param, "TestEnc::compAndSwap");

    TimeUtils timeutils;
    timeutils.start("KeyGen");
    Ring ring(param.logN, param.logQ);
    SecretKey secretKey(ring);
    Scheme scheme(secretKey, ring);
    scheme.addConjKey(secretKey);
    scheme.addLeftRotKeys(secretKey);
    scheme.addRightRotKeys(secretKey);
    timeutils.stop("KeyGen");

    double* mvec = EvaluatorUtils::randomRealArray(n);

	Ciphertext cipher = scheme.encrypt(mvec, n, param.logp, param.logQ);

	Ciphertext cipher2;

    MaskingGenerator mg(param.log2n);
    double** mask = mg.getMasking();

    timeutils.start("CompAndSwap");
    EncAlgorithm encAlgo;
    encAlgo.compAndSwap(cipher2, cipher, mask[0], 1, iter, param, scheme);
    timeutils.stop("CompAndSwap");

    for(int i = 0; i < n / 2; i++) {
        if(mvec[2 * i] > mvec[2 * i + 1]) {
            double x = mvec[2 * i];
            mvec[2 * i] = mvec[2 * i + 1];
            mvec[2 * i + 1] = x;
        }
    }
    
    cout << "comsumed logq = " << cipher.logq - cipher2.logq << endl;
	complex<double>* dvec = scheme.decrypt(secretKey, cipher2);
    PrintUtils::averageDifference(mvec, dvec, n);
}
