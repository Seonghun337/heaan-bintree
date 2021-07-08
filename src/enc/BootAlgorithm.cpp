#include "BootAlgorithm.h"

BootAlgo::BootAlgo(Parameter _param, long _sqrtIter, bool _increase) {
    param = _param;
    sqrtIter = _sqrtIter;
    invIter = sqrtIter;
    compIter = invIter;
    increase = _increase;
}

BootAlgo::BootAlgo(Parameter _param, long _invIter, long _compIter, bool _increase) {
    param = _param;
    sqrtIter = _invIter;
    invIter = _invIter;
    compIter = _compIter;
    increase = _increase;
}

BootAlgo::BootAlgo(Parameter _param, long _sqrtIter, long _invIter, long _compIter, bool _increase) {
    param = _param;
    sqrtIter = _sqrtIter;
    invIter = _invIter;
    compIter = _compIter;
    increase = _increase;
}

void BootAlgo::approxSqrt(Ciphertext& cipher, BootScheme& scheme, BootHelper& bootHelper) {
    PrintUtils::nprint("start BootAlgo::sqrt", WANT_TO_PRINT);
    
    Ciphertext b = cipher;
    long logp = param.logp;    
    scheme.addConstAndEqual(b, -1.0, logp);

    // one sqrtIteration : 2logp 
    // total = 2 * iter * logp
    Ciphertext dummy;
    for(int i = 0; i < sqrtIter; i++) {
        PrintUtils::nprint(to_string(i) + "/" + to_string(sqrtIter - 1) + "th sqrtIteration", WANT_TO_PRINT);

        scheme.checkModulusAndBoot(cipher, 2 * param.logp + 2, bootHelper, param);
        scheme.checkModulusAndBoot(b, 2 * param.logp + 2, bootHelper, param);

        // make dummy = 1 - b / 2
        dummy = scheme.divByPo2(b, 1); // b - 1
        scheme.negateAndEqual(dummy); 
        scheme.addConstAndEqual(dummy, 1.0, logp); // dummy - 1

        // Update a
        // a <- a * (1 - b / 2)
        scheme.modDownToAndEqualModified(cipher, dummy, bootHelper, param); // a - 1
        scheme.multAndEqualWithBoot(cipher, dummy, bootHelper, param);
        scheme.reScaleByAndEqual(cipher, logp); // a - logp + 1

        // make dummy = (b - 3) / 4
        dummy = scheme.addConst(b, -3.0, logp);
        scheme.divByPo2AndEqual(dummy, 2); // dummy - 3

        //update b
        // b<- b * b * (b - 3) / 4
        scheme.squareAndEuqalWithBoot(b, bootHelper, param);
        scheme.reScaleByAndEqual(b, logp); // b - logp
        scheme.modDownToAndEqualModified(dummy, b, bootHelper, param);
        scheme.multAndEqualWithBoot(b, dummy, bootHelper, param);
        scheme.reScaleByAndEqual(b, logp); // b - 2logp

        if(i < sqrtIter - 1){
            scheme.resetImagErrorAndEqual(b);
            scheme.resetImagErrorAndEqual(cipher);
            scheme.modDownToAndEqualModified(cipher, b, bootHelper, param);
        }     

        PrintUtils::nprint("After sqrtIter : logq = " + to_string(cipher.logq), WANT_TO_PRINT);
    }
    PrintUtils::nprint("end Sqrt, logq = " + to_string(cipher.logq), WANT_TO_PRINT);
}

void BootAlgo::approxSqrtDec(Ciphertext& cipher, BootScheme& scheme, BootHelper& bootHelper, SecretKey sk) {
    PrintUtils::nprint("start BootAlgo::sqrt", WANT_TO_PRINT);

    scheme.resetImagErrorAndEqual(cipher);

    scheme.checkAndBoot(cipher, true, bootHelper, param);

    // scheme.checkModulusAndBoot(cipher, 2 * param.logp + 2, bootHelper, param); // FIND_HERE
    cout << "start cipher.logq = " << cipher.logq << endl;
    Ciphertext b = cipher;
    long logp = param.logp;    
    scheme.addConstAndEqual(b, -1.0, logp);

    scheme.decryptAndPrint("start b", sk, b);
    cout << "b.logq = " << b.logq << endl;

    // one sqrtIteration : 2logp 
    // total = 2 * d * logp
    Ciphertext dummy;
    for(int i = 0; i < sqrtIter; i++) {
        PrintUtils::nprint(to_string(i) + "/" + to_string(sqrtIter - 1) + "th sqrtIteration", WANT_TO_PRINT);

        scheme.checkModulusAndBoot(cipher, 2 * param.logp + 2, bootHelper, param);
        scheme.checkModulusAndBoot(b, 2 * param.logp + 2, bootHelper, param);


        // make dummy = 1 - b / 2
        dummy = scheme.divByPo2(b, 1); // b - 1
        scheme.negateAndEqual(dummy); 
        scheme.addConstAndEqual(dummy, 1.0, logp); // dummy - 1
	scheme.decryptAndPrint(to_string(i) + "dummy = 1 - b / 2", sk, dummy);
        // Update a
        // a <- a * (1 - b / 2)
        scheme.modDownToAndEqualModified(cipher, dummy, bootHelper, param); // a - 1
        scheme.multAndEqualWithBoot(cipher, dummy, bootHelper, param);
        scheme.reScaleByAndEqual(cipher, logp); // a - logp + 1
	
        // make dummy = (b - 3) / 4
        dummy = scheme.addConst(b, -3.0, logp);
        scheme.divByPo2AndEqual(dummy, 2); // dummy - 3
	scheme.decryptAndPrint(to_string(i) + "dummy = (b - 3) / 4", sk, dummy);
        //update b
        // b<- b * b * (b - 3) / 4
        // scheme.squareByConjugateAndEqual(b, param.logp);
        scheme.squareAndEqual(b);
        scheme.reScaleByAndEqual(b, param.logp);
        scheme.modDownToAndEqualModified(dummy, b, bootHelper, param);
        scheme.multAndEqualWithBoot(b, dummy, bootHelper, param);
        scheme.reScaleByAndEqual(b, logp); // b - 2logp
	
        scheme.decryptAndPrint(to_string(i) + "b = b * b* (b-3)/4", sk, b);

        if(i < sqrtIter - 1){
            scheme.resetImagErrorAndEqual(b);
            scheme.resetImagErrorAndEqual(cipher);
            scheme.modDownToAndEqualModified(cipher, b, bootHelper, param);
        }     

        cout << "cipher.logq = " << cipher.logq << endl;
        scheme.decryptAndPrint("cipher" + to_string(i), sk, cipher);
        cout << "b.logq = " << b.logq << endl;
        scheme.decryptAndPrint("bModDown" + to_string(i), sk, b);
        PrintUtils::nprint("After sqrtIter : logq = " + to_string(cipher.logq), WANT_TO_PRINT);
    }
    PrintUtils::nprint("end Sqrt, logq = " + to_string(cipher.logq), WANT_TO_PRINT);
}

void BootAlgo::approxSqrt2(Ciphertext& cipher, BootScheme& scheme, BootHelper& bootHelper) {
     PrintUtils::nprint("start BootAlgo::sqrt", WANT_TO_PRINT);
    long logp = param.logp;  
    // We consumes modulus 2 * logp + 1 for each iteration. So check remaining modulus before copy cipher to r.
    scheme.checkAndBoot(cipher, cipher.logq - (2 * logp + 1) < param.logq, bootHelper, param);
    
    // Ciphertext r = cipher;
    // scheme.negateAndEqual(r); 
    // scheme.addAndEqual(r, cipher);
    // scheme.addConstAndEqual(r, 0.5, logp);
    double* oneHalf = new double[cipher.n];
    for (int i = 0; i < cipher.n; i++) {
        oneHalf[i] = 0.5;
    }
    Ciphertext r = scheme.encrypt(oneHalf, cipher.n, cipher.logp, cipher.logq);
    

    for(int i = 0; i < sqrtIter; i++) {
	    scheme.checkAndBoot(r, r.logq - 2 * logp - 1 < param.logq, bootHelper, param);
        scheme.checkAndBoot(cipher, cipher.logq < r.logq, bootHelper, param); // once cipher has been bootstrpped, it never bootstrapp again while the iterations end.
        
        // w <- cipher * r^3 = (cipher * r) * r^2
        Ciphertext w = scheme.modDownTo(cipher, r.logq);
        scheme.multAndEqual(w, r);
        scheme.reScaleByAndEqual(w, logp);
        Ciphertext rSquare = scheme.square(r);
        scheme.reScaleByAndEqual(rSquare, logp);
        scheme.multAndEqual(w, rSquare);
        scheme.reScaleByAndEqual(w, logp);

        // r <- (3r - w) / 2
        scheme.multByConstAndEqual(r, 3.0, 0);
        scheme.modDownByAndEqual(r, 2 * logp);
        scheme.negateAndEqual(w);
        scheme.addAndEqual(r, w);
        scheme.divByPo2AndEqual(r, 1);
    }

    scheme.checkLevelAndBoot(r, 1, bootHelper, param);
    scheme.checkAndBoot(cipher, cipher.logq < r.logq, bootHelper, param);

    scheme.modDownToAndEqualModified(cipher, r, bootHelper, param); // a - 1
    scheme.multAndEqualWithBoot(cipher, r, bootHelper, param);
    scheme.reScaleByAndEqual(cipher, logp);

    PrintUtils::nprint("end Sqrt, logq = " + to_string(cipher.logq), WANT_TO_PRINT);
}

void BootAlgo::approxSqrt2Dec(Ciphertext& cipher, BootScheme& scheme, BootHelper& bootHelper, SecretKey sk) {
    PrintUtils::nprint("start BootAlgo::sqrt", WANT_TO_PRINT);
    long logp = param.logp;  

    // We consumes modulus 3 * logp + 1 for each iteration. So check remaining modulus before copy cipher to r.
    scheme.checkAndBoot(cipher, cipher.logq - (2 * logp + 1) < param.logq, bootHelper, param);
    
    Ciphertext r = cipher;
    scheme.negateAndEqual(r); 
    scheme.addAndEqual(r, cipher);
    scheme.addConstAndEqual(r, 0.5, logp);

    scheme.decryptAndPrint("Start r", sk, r);
       
    for(int i = 0; i < sqrtIter; i++) {
	    scheme.checkAndBoot(r, r.logq - 2 * logp - 1 < param.logq, bootHelper, param);
        scheme.checkAndBoot(cipher, cipher.logq < r.logq, bootHelper, param); // once cipher has been bootstrpped, it never bootstrapp again while the iterations end.
        
        // w <- cipher * r^3 = (cipher * r) * r^2
        Ciphertext w = scheme.modDownTo(cipher, r.logq);
        scheme.multAndEqual(w, r);
        scheme.reScaleByAndEqual(w, logp);

        Ciphertext rSquare = scheme.square(r);
        scheme.reScaleByAndEqual(rSquare, logp);

        scheme.multAndEqual(w, rSquare);
        scheme.reScaleByAndEqual(w, logp);

        scheme.decryptAndPrint("w" + to_string(i), sk, w);

        // r <- (3r - w) / 2
        scheme.multByConstAndEqual(r, 3.0, 0);
        scheme.modDownToAndEqual(r, w.logq);
        scheme.negateAndEqual(w);
        scheme.addAndEqual(r, w);
        scheme.divByPo2AndEqual(r, 1);

        scheme.decryptAndPrint("r" + to_string(i), sk, r);
        // cout << "r.logq = " << r.logq << endl;
    }

    scheme.checkLevelAndBoot(r, 1, bootHelper, param);
    scheme.checkAndBoot(cipher, cipher.logq < r.logq, bootHelper, param);

    scheme.modDownToAndEqualModified(cipher, r, bootHelper, param); // a - 1
    scheme.multAndEqualWithBoot(cipher, r, bootHelper, param);
    scheme.reScaleByAndEqual(cipher, logp);

    PrintUtils::nprint("end Sqrt, logq = " + to_string(cipher.logq), WANT_TO_PRINT);
}

void BootAlgo::approxSqrt3(Ciphertext& cipher, BootScheme& scheme, BootHelper& bootHelper) {
    PrintUtils::nprint("start BootAlgo::sqrt", WANT_TO_PRINT);
    long logp = param.logp;  

    // We consumes modulus 3 * logp + 1 for each iteration. So check remaining modulus before copy cipher to r.
    scheme.checkAndBoot(cipher, cipher.logq - (3 * logp + 1) < param.logq, bootHelper, param);
    
    Ciphertext r = cipher;
    scheme.negateAndEqual(r); 
    scheme.addAndEqual(r, cipher);

    scheme.addConstAndEqual(r,0.5,logp); // set x0=1.2247
   
    Ciphertext w;
    for(int i = 0; i < sqrtIter; i++) {
        PrintUtils::nprint(to_string(i) + "/" + to_string(sqrtIter - 1) + "th iteration", WANT_TO_PRINT);
	    
	    scheme.checkAndBoot(r, r.logq - 3 * param.logp - 1 < param.logq, bootHelper, param);
        scheme.checkAndBoot(cipher, cipher.logq < r.logq, bootHelper, param); // once cipher has been bootstrpped, it never bootstrapp again while the iterations end.
        
        // w <= r**2
        w = r; // b - 1
        scheme.squareAndEuqalWithBoot(w, bootHelper, param);
        scheme.reScaleByAndEqual(w, logp); // b - logp

        // d<=1-wx
        Ciphertext d = cipher;
        scheme.modDownToAndEqualModified(d, w, bootHelper, param); // a - 1
        scheme.multAndEqualWithBoot(d, w, bootHelper, param);
        scheme.reScaleByAndEqual(d, logp);
        scheme.negateAndEqual(d); 
        scheme.addConstAndEqual(d, 1.0, logp); // d = 1-wx
        
        // update r <= r+rd/2
        scheme.modDownToAndEqualModified(r, d, bootHelper, param);
        scheme.multAndEqualWithBoot(d, r, bootHelper, param);
        scheme.reScaleByAndEqual(d, logp);
        scheme.divByPo2AndEqual(d,1);
        scheme.modDownToAndEqualModified(r, d, bootHelper, param);
        scheme.addAndEqual(r,d);
    }

    scheme.checkLevelAndBoot(r, 1, bootHelper, param);
    scheme.checkAndBoot(cipher, cipher.logq < r.logq, bootHelper, param);

    scheme.modDownToAndEqualModified(cipher, r, bootHelper, param); // a - 1
    scheme.multAndEqualWithBoot(cipher, r, bootHelper, param);
    scheme.reScaleByAndEqual(cipher, logp);

    PrintUtils::nprint("end Sqrt, logq = " + to_string(cipher.logq), WANT_TO_PRINT);
}

void BootAlgo::approxSqrt3Dec(Ciphertext& cipher, BootScheme& scheme, BootHelper& bootHelper, SecretKey sk) {
    PrintUtils::nprint("start BootAlgo::sqrt", WANT_TO_PRINT);
    long logp = param.logp;  

    // We consumes modulus 3 * logp + 1 for each iteration. So check remaining modulus before copy cipher to r.
    scheme.checkAndBoot(cipher, cipher.logq - (3 * logp + 1) < param.logq, bootHelper, param);
    
    Ciphertext r = cipher;
    scheme.negateAndEqual(r); 
    scheme.addAndEqual(r, cipher);

    scheme.addConstAndEqual(r,0.5,logp); // set x0=1.2247
   
    Ciphertext w;
    for(int i = 0; i < sqrtIter; i++) {
        PrintUtils::nprint(to_string(i) + "/" + to_string(sqrtIter - 1) + "th iteration", WANT_TO_PRINT);
	    
	    scheme.checkAndBoot(r, r.logq - 3 * param.logp - 1 < param.logq, bootHelper, param);
        scheme.checkAndBoot(cipher, cipher.logq < r.logq, bootHelper, param); // once cipher has been bootstrpped, it never bootstrapp again while the iterations end.
        
        // w <= r**2
        w = r; // b - 1
        scheme.squareAndEuqalWithBoot(w, bootHelper, param);
        scheme.reScaleByAndEqual(w, logp); // b - logp

        // d<=1-wx
        Ciphertext d = cipher;
        scheme.modDownToAndEqualModified(d, w, bootHelper, param); // a - 1
        scheme.multAndEqualWithBoot(d, w, bootHelper, param);
        scheme.reScaleByAndEqual(d, logp);
        scheme.negateAndEqual(d); 
        scheme.addConstAndEqual(d, 1.0, logp); // d = 1-wx

        scheme.decryptAndPrint("w" + to_string(i), sk, w);
        
        // update r <= r+rd/2
        scheme.modDownToAndEqualModified(r, d, bootHelper, param);
        scheme.multAndEqualWithBoot(d, r, bootHelper, param);
        scheme.reScaleByAndEqual(d, logp);
        scheme.divByPo2AndEqual(d,1);
        scheme.modDownToAndEqualModified(r, d, bootHelper, param);
        scheme.addAndEqual(r,d);

        scheme.decryptAndPrint("r" + to_string(i), sk, r);
    }

    scheme.checkLevelAndBoot(r, 1, bootHelper, param);
    scheme.checkAndBoot(cipher, cipher.logq < r.logq, bootHelper, param);

    scheme.modDownToAndEqualModified(cipher, r, bootHelper, param); // a - 1
    scheme.multAndEqualWithBoot(cipher, r, bootHelper, param);
    scheme.reScaleByAndEqual(cipher, logp);

    PrintUtils::nprint("end Sqrt, logq = " + to_string(cipher.logq), WANT_TO_PRINT);
}



void BootAlgo::approxInverse(Ciphertext& cipher, BootScheme& scheme, BootHelper& bootHelper) {
    // cout << "** Start approxInverse **" << endl;
    scheme.checkLevelAndBoot(cipher, invIter + 1, bootHelper, param);
    
    scheme.negateAndEqual(cipher);
    
    Ciphertext a = scheme.addConst(cipher, 2.0, param.logp);
    Ciphertext b = scheme.addConst(cipher, 1.0, param.logp);


    Ciphertext tmp;
    for (int i = 0; i < invIter; i++) {
        /*
             current : 
                a <- a * (1 + b^2) * (1 + b^4) *... * (1 + b^{2^{i}})
                b <- b^{2^{i}}
                and a.logq = b.logq - logp
            do :
                b <- b^2
                a <- a * (1 + b)
            this consumes 1 depth
        */
        scheme.squareAndEqual(b);
        scheme.reScaleByAndEqual(b, param.logp);

        tmp = scheme.addConst(b, 1.0, param.logp);
        if (i == 0) {
            scheme.modDownByAndEqual(a, param.logp);
        }
        scheme.multAndEqual(a, tmp);
        scheme.reScaleByAndEqual(a, param.logp);
    }
    cipher = a;
    // cout << "** End approxInverse **" << endl;
}

void BootAlgo::approxInverseWithDec(Ciphertext& cipher, BootScheme& scheme, BootHelper& bootHelper, SecretKey& secretKey) {
    scheme.negateAndEqual(cipher);
    Ciphertext a = scheme.addConst(cipher, 2.0, param.logp);
    Ciphertext b = scheme.addConst(cipher, 1.0, param.logp);
    
    for (int _ = 0; _ < invIter; _++) {
        // scheme.decryptAndPrint("a", secretKey, a);
        // scheme.decryptAndPrint("b", secretKey, b);
        complex<double>* deca = scheme.decrypt(secretKey, a);
        complex<double>* decb = scheme.decrypt(secretKey, b);

        cout << "1 : a = " << deca[1].real() << ", b = " << decb[1].real() << endl; 
        cout << "4 : a = " << deca[4].real() << ", b = " << decb[4].real() << endl; 
        // cout << "a = " << deca[6].real() << ", b = " << decb[6].real() << endl; 
        scheme.squareAndEuqalWithBoot(b, bootHelper, param);
        scheme.reScaleByAndEqual(b, param.logp);
        Ciphertext bPlusOne = scheme.addConst(b, 1.0, param.logp);
        scheme.modDownToAndEqualModified(a, bPlusOne, bootHelper, param);
        scheme.decryptAndPrint("start a", secretKey, a);
        scheme.decryptAndPrint("bPlusOne", secretKey, bPlusOne);
        cout << a.logq << ", " << bPlusOne.logq << endl;
        
        // scheme.multAndEqualWithBoot(a, bPlusOne, bootHelper, param);
        if (a.logq - 40 < 45) {
            bootHelper.bootstrapping(a, param.logq, param.logQ, param.logT);
            bootHelper.bootstrapping(bPlusOne, param.logq, param.logQ, param.logT);
            scheme.decryptAndPrint("after boot a", secretKey, a);
            scheme.decryptAndPrint("after boot bPlusOne", secretKey, bPlusOne);
        }
        
        scheme.multAndEqual(a, bPlusOne);
        scheme.reScaleByAndEqual(a, param.logp);
        scheme.decryptAndPrint("mult a", secretKey, a);
        scheme.modDownToAndEqualModified(b, a, bootHelper, param);
    }
    cipher = a;
    scheme.decryptAndPrint("last a", secretKey, cipher);
}

void BootAlgo::minMax(Ciphertext& minCipher, Ciphertext& maxCipher, BootScheme& scheme, BootHelper& bootHelper) {
    PrintUtils::nprint("start minMax with logq = " + to_string(minCipher.logq) + ", " + to_string(maxCipher.logq), WANT_TO_PRINT);

    // scheme.checkAndBoot(minCipher, minCipher.logq - param.logp - 1 < param.logq, bootHelper, param);
    // scheme.checkAndBoot(maxCipher, maxCipher.logq < minCipher.logq, bootHelper, param);
    // scheme.modDownToAndEqualModified(minCipher, maxCipher, bootHelper, param);

    Ciphertext x = scheme.add(minCipher, maxCipher);
    Ciphertext y = scheme.sub(minCipher, maxCipher);

    scheme.divByPo2AndEqual(x, 1); // x - logp + 1
    // scheme.divByPo2AndEqual(y, 1); // y - logp + 1

	scheme.resetImagErrorAndEqual(y);
    scheme.squareAndEqual(y);
    scheme.reScaleByAndEqual(y, param.logp);
    scheme.resetImagErrorAndEqual(y);   

    // sqrtCipher - (2 * sqrtIter + 1) * logp + 1
    approxSqrt(y, scheme, bootHelper);
    // approxSqrt2(y, scheme, bootHelper);
	
    scheme.divByPo2AndEqual(y, 1);

    scheme.modDownToAndEqualModified(x, y, bootHelper, param);

    maxCipher = scheme.add(x, y);
    minCipher = scheme.sub(x, y);
    PrintUtils::nprint("end minMax", WANT_TO_PRINT);
}

void BootAlgo::minMaxDec(Ciphertext& minCipher, Ciphertext& maxCipher, BootScheme& scheme, BootHelper& bootHelper, SecretKey sk) {
    PrintUtils::nprint("start minMax with logq = " + to_string(minCipher.logq) + ", " + to_string(maxCipher.logq), WANT_TO_PRINT);
    cout << "Start minMax with logq = " << minCipher.logq << endl;
    // scheme.checkAndBoot(minCipher, minCipher.logq - param.logp - 2 < param.logq, bootHelper, param);
    // scheme.checkAndBoot(maxCipher, maxCipher.logq < minCipher.logq, bootHelper, param);
    // scheme.modDownToAndEqualModified(minCipher, maxCipher, bootHelper, param);

    Ciphertext x = scheme.add(minCipher, maxCipher);
    Ciphertext y = scheme.sub(minCipher, maxCipher);

    scheme.divByPo2AndEqual(x, 1); // x - logp + 1
    // scheme.divByPo2AndEqual(y, 1); // y - logp + 1

    scheme.checkModulusAndBoot(y, param.logp + 1, bootHelper, param); //FIND_HERE
    Ciphertext yBefore = y;    

    scheme.decryptAndPrint("before square, y", sk, y);
    scheme.resetImagErrorAndEqual(y);
    scheme.squareAndEqual(y);
    scheme.reScaleByAndEqual(y, param.logp);
    scheme.resetImagErrorAndEqual(y);   
    scheme.decryptAndPrint("after square, y", sk, y);


    // scheme.addAndEqual(y, y);
    // scheme.addAndEqual(y, y);

    // sqrtCipher - (2 * sqrtIter + 1) * logp + 1
    // approxSqrt(y, scheme, bootHelper);
    cout << "sqrt, y.logq = " << y.logq << endl;
    approxSqrt2Dec(y, scheme, bootHelper, sk);
    cout << "sqrt End, y.logq = " << y.logq << endl;

    complex<double>* yBefDec = scheme.decrypt(sk, yBefore);    
    complex<double>* yAftDec = scheme.decrypt(sk, y);
    complex<double>* yBefAbs = new complex<double>[y.n];
    for (int i = 0; i < y.n; i++) {
        yBefAbs[i] = abs(yBefDec[i]);
    }

    scheme.divByPo2AndEqual(y, 1);

    cout << "after approxSqrt" << endl;
    cout << "sqrt(y) (logQ = " << yBefore.logq << ") // approxsqrt(y) (logQ = " << y.logq << ")" << endl;
    PrintUtils::printFewArrays(yBefDec, yAftDec, y.n);
    PrintUtils::averageDifference(yBefAbs, yAftDec, y.n);

    // scheme.modDownToAndEqual(x, sqrtCipher.logq);
    scheme.modDownToAndEqualModified(x, y, bootHelper, param);

    maxCipher = scheme.add(x, y);
    minCipher = scheme.sub(x, y);
    PrintUtils::nprint("end minMax", WANT_TO_PRINT);
    cout << "End minMax with logq = " << minCipher.logq << endl;

}

void BootAlgo::comparison(Ciphertext& a, Ciphertext& b, BootScheme& scheme, BootHelper& bootHelper) {
    /*
        uses approxInv for (compIter + 1) times
    */
    // cout << "** srart Comparison **" << endl;
    // cout << a.logq << ", " << b.logq << endl;    
    
    // Input messages are in (0, 1)
    scheme.addConstAndEqual(a, 0.5, param.logp);
    scheme.addConstAndEqual(b, 0.5, param.logp); // now in (1/2, 3/2)

    // normalize cipher1, cipher2
    scheme.checkAndBoot(a, a.logq - 1 < param.logq, bootHelper, param);
    scheme.checkAndBoot(b, b.logq < a.logq, bootHelper, param);
    
    // sum <- 2/(a+b) = Inv((a+b)/2)
    Ciphertext sum = scheme.add(a, b);
    scheme.divByPo2AndEqual(sum, 1);
    
    approxInverse(sum, scheme, bootHelper);
    
    // cout << "a = " << a.logq << ", inv(sum) = " << sum.logq << endl;

    // to compute a <- (a/2) * sum = a/(a+b)
    scheme.checkAndBoot(a, a.logq - param.logp - 1 < param.logq, bootHelper, param);
    scheme.checkAndBoot(sum, sum.logq - param.logp - 1 < param.logq, bootHelper, param);
    
    scheme.divByPo2AndEqual(a, 1);
    scheme.modDownToAndEqualModified(a, sum, bootHelper, param);
    scheme.multAndEqualWithBoot(a, sum, bootHelper, param);
    scheme.reScaleByAndEqual(a, param.logp);

    for (int _ = 0; _ < compIter; _++) {
        // check level for a^m
        scheme.checkModulusAndBoot(a, 3 * param.logp + 1, bootHelper, param);
        
        // Remove Im(a)
        scheme.resetImagErrorAndEqual(a);

        // update b <- 1 - a
        b = scheme.negate(a);
        scheme.addConstAndEqual(b, 1.0, param.logp);

        // Fix m = 4
        for (int i = 0; i < 2; i++) { // consumes 2 levels
            scheme.squareAndEuqalWithBoot(a, bootHelper, param);
            scheme.reScaleByAndEqual(a, param.logp);
            
            scheme.squareAndEuqalWithBoot(b, bootHelper, param);
            scheme.reScaleByAndEqual(b, param.logp);
        }
        
        Ciphertext inv = scheme.add(a, b);
        approxInverse(inv, scheme, bootHelper); // consumses invIter + 1 levels

        // cout << "a = " << a.logq << ", inv = " << inv.logq << endl;    

        scheme.checkLevelAndBoot(inv, 1, bootHelper, param);
        scheme.modDownToAndEqualModified(a, inv, bootHelper, param);

        // cout << "a = " << a.logq << ", inv = " << inv.logq << endl;    
        scheme.multAndEqualWithBoot(a, inv, bootHelper, param);
        scheme.reScaleByAndEqual(a, param.logp);
    }
    b = scheme.negate(a);
    scheme.addConstAndEqual(b, 1.0, param.logp);
    // cout << a.logq << ", " << b.logq << endl;    
    // cout << "** End Comparison ** " << endl;
}

void BootAlgo::comparisonDec(Ciphertext& a, Ciphertext& b, BootScheme& scheme, BootHelper& bootHelper, SecretKey& sk) {
    /*
        uses approxInv * (compIter + 1) times
    */
    // cout << "** srart Comparison **" << endl;
    // cout << a.logq << ", " << b.logq << endl;    
    
    // Input messages are in (0, 1)
    scheme.addConstAndEqual(a, 0.5, param.logp);
    scheme.addConstAndEqual(b, 0.5, param.logp); // now in (1/2, 3/2)

    // normalize cipher1, cipher2
    scheme.checkAndBoot(a, a.logq - 1 < param.logq, bootHelper, param);
    scheme.checkAndBoot(b, b.logq < a.logq, bootHelper, param);
    
    // sum <- 2/(a+b) = Inv((a+b)/2)
    Ciphertext sum = scheme.add(a, b);
    scheme.divByPo2AndEqual(sum, 1);

    scheme.decryptAndPrint("bofore inverse : sum", sk, sum);
    
    approxInverse(sum, scheme, bootHelper);

    scheme.decryptAndPrint("after inverse : sum", sk, sum);

    
    // cout << "a = " << a.logq << ", inv(sum) = " << sum.logq << endl;

    // to compute a <- (a/2) * sum = a/(a+b)
    scheme.checkAndBoot(a, a.logq - param.logp - 1 < param.logq, bootHelper, param);
    scheme.checkAndBoot(sum, sum.logq - param.logp - 1 < param.logq, bootHelper, param);
    
    scheme.divByPo2AndEqual(a, 1);
    scheme.modDownToAndEqualModified(a, sum, bootHelper, param);
    scheme.multAndEqualWithBoot(a, sum, bootHelper, param);
    scheme.reScaleByAndEqual(a, param.logp);

    scheme.decryptAndPrint("mult by sum, a", sk, a);


    for (int _ = 0; _ < compIter; _++) {
        // check level for a^m
        scheme.checkLevelAndBoot(a, 2, bootHelper, param);

        // update b <- 1 - a
        b = scheme.negate(a);
        scheme.addConstAndEqual(b, 1.0, param.logp);

        scheme.decryptAndPrint("before squaring, a", sk, a);
        scheme.decryptAndPrint("before squaring, b", sk, b);

        // Fix m = 4
        for (int i = 0; i < 2; i++) { // consumes 2 levels
            scheme.squareAndEuqalWithBoot(a, bootHelper, param);
            scheme.reScaleByAndEqual(a, param.logp);
            
            scheme.squareAndEuqalWithBoot(b, bootHelper, param);
            scheme.reScaleByAndEqual(b, param.logp);
        }
        scheme.decryptAndPrintAll("after squaring, a", sk, a);
        scheme.decryptAndPrint("after squaring, b", sk, b);
        
        Ciphertext inv = scheme.add(a, b);

        scheme.decryptAndPrint("before inv, inv", sk, inv);
        cout << "inv.logq = " << inv.logq << endl;
        if(inv.logq - (invIter + 2) * param.logp < param.logq) {
            scheme.checkAndBoot(inv, true, bootHelper,param);
            scheme.decryptAndPrint("Bootstrap inv, inv", sk, inv);
            cout << "inv.logq = " << inv.logq << endl;
        }
        approxInverse(inv, scheme, bootHelper); // consumses invIter + 1 levels

        scheme.decryptAndPrint("after inv, inv", sk, inv);
        cout << "inv.logq = " << inv.logq << endl;

        cout << "a = " << a.logq << ", inv = " << inv.logq << endl;    

        
        if(a.logq - param.logp < param.logq || inv.logq - param.logp < param.logq) {
            scheme.checkAndBoot(a, true, bootHelper, param);
            scheme.checkAndBoot(inv, true, bootHelper, param);
        }

        scheme.modDownToAndEqualModified(a, inv, bootHelper, param);

        cout << "a = " << a.logq << ", inv = " << inv.logq << endl;    

        scheme.multAndEqualWithBoot(a, inv, bootHelper, param);
        scheme.reScaleByAndEqual(a, param.logp);

        scheme.decryptAndPrintAll("Iteration : final a", sk, a);
    }
    b = scheme.negate(a);
    scheme.addConstAndEqual(b, 1.0, param.logp);
    // cout << a.logq << ", " << b.logq << endl;    
    // cout << "** End Comparison ** " << endl;
}

void BootAlgo::compAndSwap(Ciphertext& cipher, double** mask, long loc, long dist, BootScheme& scheme, Ring& ring, BootHelper& bootHelper) {
    PrintUtils::nprint("start compAndSwap with logq = " + to_string(cipher.logq), WANT_TO_PRINT);

    long n = cipher.n;
    ZZ* maskPoly = new ZZ[1 << param.logN];
    ring.encode(maskPoly, mask[loc], cipher.n, param.logp);
    // ring.encode(maskPoly, mask[loc], cipher.n, param.logp);

    scheme.checkModulusAndBoot(cipher, 3 * param.logp, bootHelper, param);

    Ciphertext dummy = cipher;
    scheme.multByPolyAndEqualWithBoot(dummy, maskPoly, bootHelper, param);
    scheme.reScaleByAndEqual(dummy, param.logp);
    // scheme.reScaleByAndEqual(dummy, param.logp);
    scheme.modDownToAndEqualModified(cipher, dummy, bootHelper, param);
    scheme.subAndEqual(cipher, dummy);

    scheme.rightRotateAndEqualConditional(dummy, dist, increase);

    double* maskDummy = new double[n];
    for (int i = 0; i < n; i++) {
        maskDummy[i] = mask[loc][i] * 0.0001;
    }
    ZZ* maskDummyPoly = new ZZ[1 << param.logN];
    ring.encode(maskDummyPoly, maskDummy, n, param.logp);

    scheme.addByPolyAndEqual(cipher, maskDummyPoly, param.logp);
    minMax(dummy, cipher, scheme, bootHelper);
    scheme.subByPolyAndEqual(cipher, maskDummyPoly, param.logp);
    
    scheme.leftRotateAndEqualConditional(dummy, dist, increase); 
        
    scheme.addAndEqual(cipher, dummy);   

    PrintUtils::nprint("end compAndSwap", WANT_TO_PRINT);
}

void BootAlgo::compAndSwapDec(Ciphertext& cipher, double* mask, long dist, BootScheme& scheme, Ring& ring, BootHelper& bootHelper, long loc, SecretKey sk) {
    PrintUtils::nprint("start compAndSwap with logq = " + to_string(cipher.logq), WANT_TO_PRINT);

    long n = cipher.n;
    ZZ* maskPoly = new ZZ[1 << param.logN];
    ring.encode(maskPoly, mask, cipher.n, param.logp);


    // scheme.checkModulusAndBoot(cipher, param.logp + 1, bootHelper, param);
    cout << "boot at start of compAndSwap?" << endl;
    scheme.checkModulusAndBoot(cipher, 3 * param.logp, bootHelper, param);

    scheme.decryptAndPrint("before reset imag, cipher", sk, cipher);
    
    
    Ciphertext dummy = cipher;
    scheme.multByPolyAndEqualWithBoot(dummy, maskPoly, bootHelper, param);
    scheme.reScaleByAndEqual(dummy, param.logp);
    // scheme.reScaleByAndEqual(dummy, param.logp);
    scheme.modDownToAndEqualModified(cipher, dummy, bootHelper, param);
    scheme.subAndEqual(cipher, dummy);

    scheme.rightRotateAndEqualConditional(dummy, dist, increase);

    double* maskDummy = new double[n];
    for (int i = 0; i < n; i++) {
        maskDummy[i] = mask[i] * 0.0001;
    }
    ZZ* maskDummyPoly = new ZZ[1 << param.logN];
    ring.encode(maskDummyPoly, maskDummy, n, param.logp);
    
    scheme.addByPolyAndEqual(cipher, maskDummyPoly, param.logp);

    scheme.decryptAndPrint("before minmax, cipher", sk, cipher);
    scheme.decryptAndPrint("before minmax, dummy", sk, dummy);

    minMaxDec(dummy, cipher, scheme, bootHelper, sk);

    scheme.decryptAndPrint("after minmax, cipher", sk, cipher);
    scheme.decryptAndPrint("after minmax, dummy", sk, dummy);

    scheme.subByPolyAndEqual(cipher, maskDummyPoly, param.logp);
    
    scheme.leftRotateAndEqualConditional(dummy, dist, increase); 
    
    scheme.addAndEqual(cipher, dummy);   

    PrintUtils::nprint("end compAndSwap", WANT_TO_PRINT);
}

void BootAlgo::selfBitonicMerge(Ciphertext& cipher, double** mask, BootScheme& scheme, Ring& ring, BootHelper& bootHelper) {
    for(int i = 0; i < param.log2n; i++) {
        cout << "                   - selfBitonicMerge, " << param.log2n - 1 - i << endl;
        compAndSwap(cipher, mask, i, 1 << (param.log2n - 1 - i), scheme, ring, bootHelper);
    }
}

void BootAlgo::selfTableMerge(Ciphertext& cipher, long logDataNum, long colNum, double*** mask, BootScheme& scheme, Ring& ring, BootHelper& bootHelper, SecretKey sk) {
    for(int i = 0; i < param.log2n - logDataNum; i++) {
        cout << "                   - selfBitonicMerge, " << param.log2n - 1 - i << endl;
        compAndSwapTable(cipher, logDataNum, colNum, mask[0][i], mask[1][i], mask[2][i], mask[3][i], 1 << (param.log2n - 1 - i), scheme, ring, bootHelper, sk);
    }
}

void BootAlgo::reverse(Ciphertext& cipher, double** mask, BootScheme& scheme, Ring& ring, BootHelper& bootHelper) {
    scheme.leftRotateFastAndEqual(cipher, 1 << (param.log2n - 1));
    for(int i = 1; i < param.log2n; i++) {
        ZZ* maskPoly = new ZZ[1 << param.logN];
        ring.encode(maskPoly, mask[i], cipher.n, param.logp);
        // ring.encode(maskPoly, mask[i], cipher.n, param.logp);
        Ciphertext dummy = cipher;
        scheme.multByPolyAndEqualWithBoot(dummy, maskPoly, bootHelper, param);
        scheme.reScaleByAndEqual(dummy, param.logp);
        // scheme.reScaleByAndEqual(dummy, param.logp);
        scheme.modDownToAndEqualModified(cipher, dummy, bootHelper, param);
        scheme.subAndEqual(cipher, dummy);

        scheme.leftRotateFastAndEqual(cipher, 1 << (param.log2n - i - 1));
        scheme.rightRotateFastAndEqual(dummy, 1 << (param.log2n - i - 1));
        scheme.addAndEqual(cipher, dummy);
    }
}

void BootAlgo::reverse(Ciphertext& cipher, double** mask, double** maskRight, long level, BootScheme& scheme, Ring& ring, BootHelper& bootHelper) {
    for(int i = 1; i <= level; i++) {
        cout << "i = " << i << endl;
        ZZ* maskLeftPoly = new ZZ[1 << param.logN];
        ZZ* maskRightPoly = new ZZ[1 << param.logN];
        ring.encode(maskLeftPoly, mask[i-1], cipher.n, param.logp);
        ring.encode(maskRightPoly, maskRight[i-1], cipher.n, param.logp);
        
        scheme.checkModulusAndBoot(cipher, param.logp + 1, bootHelper, param);
        scheme.resetImagErrorAndEqual(cipher);

        Ciphertext left = scheme.multByPoly(cipher, maskLeftPoly, param.logp);
        Ciphertext right = scheme.multByPoly(cipher, maskRightPoly, param.logp);
        scheme.reScaleByAndEqual(left, param.logp);
        scheme.reScaleByAndEqual(right, param.logp);
        scheme.modDownToAndEqualModified(cipher, left, bootHelper, param);
        scheme.subAndEqual(cipher, left);
        scheme.subAndEqual(cipher, right);
        cout << "rotate by " << (1 << (level - i)) << endl;
        scheme.rightRotateAndEqualConditional(left, 1 << (level - i), increase);
        scheme.leftRotateAndEqualConditional(right, 1 << (level - i), increase);
        scheme.addAndEqual(cipher, left);
        scheme.addAndEqual(cipher, right);
    }
}

void BootAlgo::compAndSwapTable(Ciphertext& cipher, long logDataNum, long colNum, double* mask, double* maskRight, double* maskTable, double* maskTableRight, long dist, BootScheme& scheme, Ring& ring, BootHelper& bootHelper, SecretKey& secretKey) {
    // cout << "start compAndSwapTable with dist = " << dist << endl; 
    scheme.checkLevelAndBoot(cipher, 3, bootHelper, param);

    ZZ* maskPoly = new ZZ[1 << param.logN];
    ZZ* maskRightPoly = new ZZ[1 << param.logN];
    ZZ* maskTablePoly = new ZZ[1 << param.logN];
    ZZ* maskTableRightPoly = new ZZ[1 << param.logN];
    ring.encode(maskPoly,           mask,           cipher.n, param.logp);
    ring.encode(maskRightPoly,      maskRight,      cipher.n, param.logp);
    ring.encode(maskTablePoly,      maskTable,      cipher.n, param.logp);
    ring.encode(maskTableRightPoly, maskTableRight, cipher.n, param.logp);
    
    Ciphertext cipher1 = scheme.multByPolyWithBoot(cipher, maskPoly, bootHelper, param);
    Ciphertext cipher1Right = scheme.multByPolyWithBoot(cipher, maskRightPoly, bootHelper, param);
    Ciphertext cipherTable = scheme.multByPolyWithBoot(cipher, maskTablePoly, bootHelper, param);
    Ciphertext cipherTableRight = scheme.multByPolyWithBoot(cipher, maskTableRightPoly, bootHelper, param); 
    scheme.reScaleByAndEqual(cipher1, param.logp);
    scheme.reScaleByAndEqual(cipher1Right, param.logp);
    scheme.reScaleByAndEqual(cipherTable, param.logp);
    scheme.reScaleByAndEqual(cipherTableRight, param.logp);

    scheme.modDownToAndEqual(cipher, cipher1.logq);
    scheme.subAndEqual(cipher, cipher1);
    scheme.subAndEqual(cipher, cipher1Right);

    scheme.rightRotateAndEqualConditional(cipher1, dist, increase);
    scheme.rightRotateAndEqualConditional(cipherTable, dist, increase);

    minMaxTable(cipher1, cipher1Right, cipherTable, cipherTableRight, logDataNum, colNum, maskRightPoly, maskTableRightPoly, scheme, ring, bootHelper, secretKey);    

    scheme.leftRotateAndEqualConditional(cipher1, dist, increase);

    scheme.modDownToAndEqual(cipher, cipher1.logq);

    scheme.addAndEqual(cipher, cipher1);
    scheme.addAndEqual(cipher, cipher1Right);
}

void BootAlgo::minMaxTable(Ciphertext& minCipher, Ciphertext& maxCipher, Ciphertext& minCipherTable, Ciphertext& maxCipherTable, long logDataNum, long colNum, ZZ* maskPoly, ZZ* maskTablePoly, BootScheme& scheme, Ring& ring, BootHelper& bootHelper, SecretKey& secretKey) {
    /*
        Inputs:
            minCipher : (a, a1, a2, a3), ( ... )
            maxCipher : (b, b1, b2, b3), ( ... )
            minCipherTable : (a, 0, 0, 0), ( ... )
            maxCipherTable : (b, 0, 0, 0), ( ... )
    */
    // cout << "start minMaxTable with" << endl;
    // scheme.decryptAndPrint("minTable", secretKey, minCipherTable);
    // scheme.decryptAndPrint("maxTable", secretKey, maxCipherTable);

    // comparisonDec(minCipherTable, maxCipherTable, scheme, bootHelper, secretKey);
    comparison(minCipherTable, maxCipherTable, scheme, bootHelper);
    /*
        after comp :
            minCipherTable : (comp(a,b), *, *, *), ( ... )
            maxCipherTable : (comp(b,a), *, *, *), ( ... )
    */

    // cout << "after comp" << endl;
    // scheme.decryptAndPrint("minTable", secretKey, minCipherTable);
    // scheme.decryptAndPrint("maxTable", secretKey, maxCipherTable);

    scheme.checkLevelAndBoot(minCipherTable, 2, bootHelper, param);
    scheme.checkLevelAndBoot(maxCipherTable, 2, bootHelper, param);
    scheme.checkLevelAndBoot(minCipher, 1, bootHelper, param);
    scheme.checkLevelAndBoot(maxCipher, 1, bootHelper, param);

    // mult maskTablePoly = (1, 0, 0, 0) to remove * parts
    scheme.multByPolyAndEqualWithBoot(minCipherTable, maskTablePoly, bootHelper, param);
    scheme.multByPolyAndEqualWithBoot(maxCipherTable, maskTablePoly, bootHelper, param);
    scheme.reScaleByAndEqual(minCipherTable, param.logp);
    scheme.reScaleByAndEqual(maxCipherTable, param.logp);

    /*
        after comp :
            minCipherTable : (comp(a,b), 0, 0, 0), ( ... )
            maxCipherTable : (comp(b,a), 0, 0, 0), ( ... )
    */

    // cout << "after masking" << endl;
    // scheme.decryptAndPrint("minTable", secretKey, minCipherTable);
    // scheme.decryptAndPrint("maxTable", secretKey, maxCipherTable);


    /*
        copy :
            minCipherTable : (comp(a,b), comp(a,b), comp(a,b), comp(a,b)), ( ... )
            maxCipherTable : (comp(b,a), comp(b,a), comp(b,a), comp(b,a)), ( ... )
    */
    scheme.leftRotateFastAndEqual(minCipherTable, colNum);
    scheme.leftRotateFastAndEqual(maxCipherTable, colNum);

    
    for (int i = 0; i < logDataNum; i++) {
        Ciphertext tmpMinTable = scheme.rightRotateFast(minCipherTable, 1 << i);
        Ciphertext tmpMaxTable = scheme.rightRotateFast(maxCipherTable, 1 << i);
        scheme.addAndEqual(minCipherTable, tmpMinTable);
        scheme.addAndEqual(maxCipherTable, tmpMaxTable);
    }

    // cout << "after rot" << endl;
    // scheme.decryptAndPrint("minTable", secretKey, minCipherTable);
    // scheme.decryptAndPrint("maxTable", secretKey, maxCipherTable);

    // Flip <- 1 - Table
    Ciphertext minCipherTableFlip = scheme.negate(minCipherTable);
    Ciphertext maxCipherTableFlip = scheme.negate(maxCipherTable);

    scheme.addByPolyAndEqual(minCipherTableFlip, maskPoly, param.logp);
    scheme.addByPolyAndEqual(maxCipherTableFlip, maskPoly, param.logp);
    
    // scheme.addByPolyAndEqual(cipherTableFlip, maskPoly, param.logp);
    // scheme.addByPolyAndEqual(cipherTableFlipRight, maskRightPoly, param.logp);


    // scheme.decryptAndPrint("tableFlip", secretKey, minCipherTableFlip);
    // scheme.decryptAndPrint("tableFlipRight", secretKey, maxCipherTableFlip);

    if (minCipher.logq < minCipherTable.logq) {
        scheme.modDownToAndEqual(minCipherTable, minCipher.logq);
        scheme.modDownToAndEqual(minCipherTableFlip, minCipher.logq);
        scheme.modDownToAndEqual(maxCipherTable, minCipher.logq);
        scheme.modDownToAndEqual(maxCipherTableFlip, minCipher.logq);
    } else {
        scheme.modDownToAndEqual(minCipher, minCipherTable.logq);
        scheme.modDownToAndEqual(maxCipher, minCipherTable.logq);
    }
        
    Ciphertext minCipherSmall = scheme.multWithBoot(minCipher, minCipherTableFlip, bootHelper, param);
    Ciphertext minCipherBig = scheme.multWithBoot(minCipher, minCipherTable, bootHelper, param);
    Ciphertext maxCipherSmall = scheme.multWithBoot(maxCipher, maxCipherTableFlip, bootHelper, param);
    Ciphertext maxCipherBig = scheme.multWithBoot(maxCipher, maxCipherTable, bootHelper, param);
    scheme.reScaleByAndEqual(minCipherSmall, param.logp);
    scheme.reScaleByAndEqual(minCipherBig, param.logp);
    scheme.reScaleByAndEqual(maxCipherSmall, param.logp);
    scheme.reScaleByAndEqual(maxCipherBig, param.logp);

    // cout << "check" << endl;
    // scheme.decryptAndPrint("minSmall", secretKey, minCipherSmall);
    // scheme.decryptAndPrint("minBig", secretKey, minCipherBig);
    // scheme.decryptAndPrint("maxSmall", secretKey, maxCipherSmall);
    // scheme.decryptAndPrint("maxBig", secretKey, maxCipherBig);

    minCipher = scheme.add(minCipherSmall, maxCipherSmall);
    maxCipher = scheme.add(minCipherBig, maxCipherBig);

    // cout << "result" << endl;
    // scheme.decryptAndPrint("minCipher", secretKey, minCipher);
    // scheme.decryptAndPrint("maxCipher", secretKey, maxCipher);
}


void BootAlgo::halfCleaner(Ciphertext& cipher, double* mask, long dist, BootScheme& scheme, Ring& ring, BootHelper& bootHelper, SecretKey sk) {
    ZZ* maskPoly = new ZZ[1 << param.logN];
    ring.encode(maskPoly, mask, cipher.n, param.logp);

    scheme.checkLevelAndBoot(cipher, 1, bootHelper, param);
        
    Ciphertext dummy = scheme.multByPoly(cipher, maskPoly, param.logp);
    scheme.reScaleByAndEqual(dummy, param.logp);
    scheme.modDownByAndEqual(cipher, param.logp);
    scheme.subAndEqual(cipher, dummy);
    
    long n = cipher.n;
    double* maskDummy = new double[n];
    
    for (int i = 0; i < n; i++) {
        maskDummy[i] = mask[i] / ((double) (1 << (cipher.n-2)));
    }
    ZZ* maskDummyPoly = new ZZ[1 << param.logN];
    ring.encode(maskDummyPoly, maskDummy, n, param.logp);
    
    scheme.addByPolyAndEqual(cipher, maskDummyPoly, param.logp);

    scheme.rightRotateAndEqualConditional(dummy, dist, increase);
    minMaxDec(dummy, cipher, scheme, bootHelper, sk);
    scheme.leftRotateAndEqualConditional(dummy, dist, increase);
    
    scheme.subByPolyAndEqual(cipher, maskDummyPoly, param.logp);
    
    scheme.addAndEqual(cipher, dummy);
}
