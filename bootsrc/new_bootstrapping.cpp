#include "new_bootstrapping.h"

BootHelper::BootHelper(long logSlots, long radix, long logp, Scheme& scheme, Ring& ring, SecretKey& secretKey)
	 : scheme(scheme), ring(ring), logSlots(logSlots), radix(radix), logp(logp)
{
	// parameters		
	slots = 1 << logSlots;
	log2r = log2(radix);
	logrSlots = logSlots / log2r;
	
	// decompose V0 matrix and invV0
	V0Decomp = new MatrixXc[logSlots]();
	invV0Decomp = new MatrixXc[logSlots]();
	buildV0DecompMatrix(V0Decomp, slots);
	for(long i = 0; i < logSlots; i++) {
		invV0Decomp[i] = V0Decomp[i].transpose();
		conjugate(invV0Decomp[i]);
		divideBy(invV0Decomp[i], 2.);
	}
	
	// make public keys for left rotations
	for(long i = 0; i < logrSlots; i++) {
		long gap = slots / (radix * pow(radix, i));
		if(i == 0) {
			if(radix > 4) {
				long bs = 1 << (long)floor((log2(radix)) / 2.);
				long gs = (radix) / bs;
				for(long j = 1; j < bs; j++) {
					long idx = j * gap;
					idx %= slots;
					if(idx != 0) scheme.addLeftRotKey(secretKey, idx);
				}
				for(long j = 1; j < gs; j++) {
					long idx = j * gap * bs;
					idx %= slots;
					if(idx != 0) scheme.addLeftRotKey(secretKey, idx);
				}
			}
			else {
				for(long j = 1; j < radix; j++) {
					long idx = j * gap;
					idx %= slots;
					if(idx != 0) scheme.addLeftRotKey(secretKey, idx);
				}
			}
		}
		else {
			scheme.addRightRotKey(secretKey, (radix - 1) * gap);
			if(radix > 2) {
				long bs = 1 << (long)floor((log2(radix) + 1) / 2.);
				long gs = (2 * radix) / bs;
				for(long j = 1; j < bs; j++) {
					long idx = j * gap;
					idx %= slots;
					if(idx != 0) scheme.addLeftRotKey(secretKey, idx);
				}
				for(long j = 1; j < gs; j++) {
					long idx = j * gap * bs;
					idx %= slots;
					if(idx != 0) scheme.addLeftRotKey(secretKey, idx);
				}
			}
			else {
				for(long j = 1; j < 2 * radix - 1; j++) {
					long idx = j * gap;
					idx %= slots;
					if(idx != 0) scheme.addLeftRotKey(secretKey, idx);
				}
			}
		}
	}
	
	// encode diagonals for invV0 //
	encodeinvV0poly = new ZZ**[logrSlots];
	for(long i = 0; i < logrSlots; i++) {
		for(long j = 1; j < log2r; j++)
			invV0Decomp[i * log2r] = invV0Decomp[i * log2r + j] * invV0Decomp[i * log2r];
		if(i == 0) {
			encodeinvV0poly[0] = new ZZ*[radix];
			for(long j = 0; j < radix; j++) {
				encodeinvV0poly[0][j] = new ZZ[ring.N]();
				long gap = slots / radix;
				encodeDiagonal(encodeinvV0poly[0][j], invV0Decomp[0], j * gap);
			}
		} else {
			encodeinvV0poly[i] = new ZZ*[2 * radix - 1];
			long gap = slots / (radix * pow(radix, i));
			for(long j = 0; j < 2 * radix - 1; j++) {
				encodeinvV0poly[i][j] = new ZZ[ring.N]();
				encodeDiagonal(encodeinvV0poly[i][j], invV0Decomp[i * log2r], (-radix + 1 + j) * gap);
			}
		}
	}

	// encode diagonals for V0 //
	encodeV0poly = new ZZ**[logrSlots];
	for(long i = 0; i < logrSlots; i++) {
		MatrixXc tmpMat = invV0Decomp[i * log2r].transpose();
		conjugate(tmpMat);
		divideBy(tmpMat, 1. / (double)radix);
		if(i == 0) {
			encodeV0poly[0] = new ZZ*[radix];
			for(long j = 0; j < radix; j++) {
				encodeV0poly[0][j] = new ZZ[ring.N]();
				long gap = slots / radix;
				encodeDiagonal(encodeV0poly[0][j], tmpMat, j * gap);
			}
		} else {
			encodeV0poly[i] = new ZZ*[2 * radix - 1];
			long gap = slots / (radix * pow(radix, i));
			for(long j = 0; j < 2 * radix - 1; j++) {
				encodeV0poly[i][j] = new ZZ[ring.N]();
				encodeDiagonal(encodeV0poly[i][j], tmpMat, (-radix + 1 + j) * gap);
			}
		}
	}

	// pre-compute
	ZZ* tmpPoly = new ZZ[ring.N](); 
	for(long i = 0; i < logrSlots; i++) {
		long bs, gs;
		long gap = slots / (radix * pow(radix, i)); 
		if(i == 0) {
			if(radix > 4) {
				long bs = 1 << (long)floor((log2(radix)) / 2.);
				long gs = (radix) / bs;
				for(long j = 1; j < gs; ++j) {
					for(long k = 0; k < bs; ++k) {
						ring.leftRotate(tmpPoly, encodeinvV0poly[0][j*bs+k],slots-j*bs*gap);
						for(long l = 0; l < ring.N; ++l)
							encodeinvV0poly[0][j*bs+k][l] = tmpPoly[l];
						ring.leftRotate(tmpPoly, encodeV0poly[0][j*bs+k],slots-j*bs*gap);
						for(long l = 0; l < ring.N; ++l)
							encodeV0poly[0][j*bs+k][l] = tmpPoly[l];
					}
				}
			}
		} 
		else {
			if(radix > 2) {
				long bs = 1 << (long)floor((log2(radix) + 1) / 2.);
				long gs = (2 * radix) / bs;
				for(long j = 1; j < gs; ++j) {
					for(long k = 0; k < bs; ++k) {
						if((j * bs + k) < (2 * radix - 1)) {
							ring.leftRotate(tmpPoly, encodeinvV0poly[i][j*bs+k],slots-j*bs*gap);
							for(long l = 0; l < ring.N; ++l)
								encodeinvV0poly[i][j*bs+k][l] = tmpPoly[l];
							ring.leftRotate(tmpPoly, encodeV0poly[i][j*bs+k],slots-j*bs*gap);
							for(long l = 0; l < ring.N; ++l)
								encodeV0poly[i][j*bs+k][l] = tmpPoly[l];
						}
					}
				}
			}
		}
	}
	delete[] tmpPoly;
}

BootHelper::~BootHelper() {
	delete[] V0Decomp;
	for(long i = 0; i < logrSlots; i++) {
		if(i == 0) {
			for(long j = 0; j < radix; j++) delete[] encodeV0poly[i][j];
		}
		else {
			for(long j = 0; j < 2 * radix - 1; j++) delete[] encodeV0poly[i][j];
		}
		delete[] encodeV0poly[i];
	}
	delete[] encodeV0poly;
}

void BootHelper::buildPartialMatrix(MatrixXc& A, long start_row, long start_col, long size)
{
	long hsize = size / 2;
	long qsize = 4 * size;
	complex<double> omega = exp(complex<double>(0, 2 * M_PI / qsize));
	long powerofFive = 1;
	for(long i = 0; i < hsize; i++) {
		A.coeffRef(start_row + i, start_col + i) = 1.;
		A.coeffRef(start_row + hsize + i, start_col + i) = 1.;
		A.coeffRef(start_row + i, start_col + hsize + i) = pow(omega, powerofFive);
		A.coeffRef(start_row + hsize + i, start_col + hsize + i) = -pow(omega, powerofFive);
		powerofFive *= 5;
		powerofFive %= qsize;
	}
	return;
}

void BootHelper::buildV0DecompMatrix(MatrixXc* V0Decomp, long n)
{
	long log2n = (long)log2(n);
	for(long i = 0; i < log2n; i++) {
		V0Decomp[i].resize(n, n);
		for(long j = 0; j < (1 << i); j++) {
			long size = n / (1 << i);
			buildPartialMatrix(V0Decomp[i], size * j, size * j, size);
		}
	}
	return;
}

void BootHelper::encodeDiagonal(ZZ* poly, MatrixXc A, long idx)
{
	long col = A.cols();
	long row = A.rows();
	complex<double>* vec = new complex<double>[row]();
	for(long i = 0; i < row; i++) {
		long tmp = i + idx;
		while(1) {
			if(tmp < 0) tmp += col;
			else break;
		}
		tmp %= col;
		vec[i] = A.coeff(i, tmp);
	}
	ring.encode(poly, vec, row, logp);
	delete[] vec;
	return;
}

void BootHelper::invV0(Ciphertext& cipher)
{
	ZZ* tmpPoly = new ZZ[ring.N]();
	Ciphertext tmpCipher;

	// first step //
	long gap = slots / radix;
	long bs = 1 << (long)floor((log2(radix)) / 2.);
	long gs = (radix) / bs;
	if(radix > 4) {
		// In this case, we use basy-step giant-step method
		Ciphertext* bsrotFirst = new Ciphertext[bs];
		Ciphertext* gsrotFirst = new Ciphertext[gs];		
		bsrotFirst[0] = cipher;
		
		NTL_EXEC_RANGE(bs-1,first,last)

		for(long i = first; i < last; ++i) { // baby-step
			bsrotFirst[i+1] = scheme.leftRotateFast(cipher, (i+1)*gap);
		}

		NTL_EXEC_RANGE_END

		NTL_EXEC_RANGE(gs,first,last)

		for(long i = first; i < last; i++) {
			gsrotFirst[i] = scheme.multByPoly(bsrotFirst[0], encodeinvV0poly[0][i*bs], logp);
			for(long j = 1; j < bs; j++) {
				Ciphertext tmp = scheme.multByPoly(bsrotFirst[j], encodeinvV0poly[0][i*bs+j], logp);
				scheme.addAndEqual(gsrotFirst[i], tmp);
			}
		}

		NTL_EXEC_RANGE_END

		cipher = gsrotFirst[0];
		for(long i = 0; i < gs-1; ++i) { // giant-step
			scheme.leftRotateFastAndEqual(gsrotFirst[i+1], (i+1)*bs*gap);
		}
		for(long i = 1; i < gs; i++) {
			scheme.addAndEqual(cipher, gsrotFirst[i]);
		}
		delete[] bsrotFirst;
		delete[] gsrotFirst;
	} else {
		tmpCipher = cipher;
		scheme.multByPolyAndEqual(cipher, encodeinvV0poly[0][0], logp);
		for(long i = 1; i < radix; i++) {
			long gap = slots / radix;
			Ciphertext rot = scheme.leftRotateFast(tmpCipher, i * gap);
			scheme.multByPolyAndEqual(rot, encodeinvV0poly[0][i], logp);
			scheme.addAndEqual(cipher, rot);
		}
	}
	scheme.reScaleByAndEqual(cipher, logp);

	// other steps //
	if(radix > 2) {
		// In this case, we use baby-step giant-step method //
		bs = 1 << (long)floor((log2(radix) + 1) / 2.);
		gs = (2 * radix) / bs;
		Ciphertext* bsrot = new Ciphertext[bs];
		Ciphertext* gsrot = new Ciphertext[gs];
		for(long i = 1; i < logrSlots; i++) {
			gap = slots / (radix * pow(radix, i));
			tmpCipher = scheme.rightRotateFast(cipher, (radix - 1) * gap);
			bsrot[0] = tmpCipher;

			NTL_EXEC_RANGE(bs-1,first,last)

			for(long j = first; j < last; j++) { // baby-step
				bsrot[j+1] = scheme.leftRotateFast(tmpCipher, (j+1) * gap);
			}

			NTL_EXEC_RANGE_END

			NTL_EXEC_RANGE(gs,first,last)
	
			for(long j = first; j < last; j++) {
				gsrot[j] = scheme.multByPoly(bsrot[0], encodeinvV0poly[i][j*bs], logp);
				for(long k = 1; k < bs; k++) {
					if((j * bs + k) < (2 * radix - 1)) {
						Ciphertext tmp = scheme.multByPoly(bsrot[k], encodeinvV0poly[i][j*bs+k], logp);
						scheme.addAndEqual(gsrot[j], tmp);
					}
				}
			}

			NTL_EXEC_RANGE_END

			cipher = gsrot[0];
			
			NTL_EXEC_RANGE(gs-1,first,last)

			for(long j = first; j < last; ++j) { // giant-step
				scheme.leftRotateFastAndEqual(gsrot[j+1], (j+1)*bs*gap);
			}

			NTL_EXEC_RANGE_END

			for(long j = 1; j < gs; j++) {
				scheme.addAndEqual(cipher, gsrot[j]);
			}
			scheme.reScaleByAndEqual(cipher, logp);		
		}
		delete[] gsrot;
		delete[] bsrot;
	} else {
		for(long i = 1; i < logrSlots; i++) {
			gap = slots / (radix * pow(radix, i));
			cipher = scheme.rightRotateFast(cipher, (radix - 1) * gap);
			tmpCipher = cipher;
			scheme.multByPolyAndEqual(cipher, encodeinvV0poly[i][0], logp);
			for(long j = 1; j < (2 * radix - 1); j++) {
				Ciphertext rot = scheme.leftRotateFast(tmpCipher, j * gap);
				scheme.multByPolyAndEqual(rot, encodeinvV0poly[i][j], logp);
				scheme.addAndEqual(cipher, rot);
			}
			scheme.reScaleByAndEqual(cipher, logp);
		}
	}
	return;
}

void BootHelper::V0(Ciphertext& cipher)
{
	ZZ* tmpPoly = new ZZ[ring.N]();
	Ciphertext tmpCipher;
	long bs, gs;

	// other step //
	if(radix > 2) {
		// In this case, we use baby-step giant-step method //
		bs = 1 << (long)floor((log2(radix) + 1) / 2.);
		gs = (2 * radix) / bs;
		Ciphertext* bsrot = new Ciphertext[bs];
		Ciphertext* gsrot = new Ciphertext[gs];
		for(long i = 1; i < logrSlots; i++) {
			long gap = slots / (radix * pow(radix, logrSlots-i));
			tmpCipher = scheme.rightRotateFast(cipher, (radix - 1) * gap);
			bsrot[0] = tmpCipher;

			NTL_EXEC_RANGE(bs-1,first,last)

			for(long j = first; j < last; j++) { // baby-step
				bsrot[j+1] = scheme.leftRotateFast(tmpCipher, (j+1) * gap);
			}

			NTL_EXEC_RANGE_END
			
			NTL_EXEC_RANGE(gs,first,last)

			for(long j = first; j < last; j++) {
				gsrot[j] = scheme.multByPoly(bsrot[0], encodeV0poly[logrSlots-i][j*bs], logp);
				for(long k = 1; k < bs; k++) {
					if((j*bs+k) < (2*radix-1)) {
						Ciphertext tmp = scheme.multByPoly(bsrot[k], encodeV0poly[logrSlots-i][j*bs+k], logp);
						scheme.addAndEqual(gsrot[j], tmp);
					}
				}
			}

			NTL_EXEC_RANGE_END

			cipher = gsrot[0];
			for(long j = 1; j < gs; j++) { // giant-step
				Ciphertext rot = scheme.leftRotateFast(gsrot[j], j*bs*gap);
				scheme.addAndEqual(cipher, rot);
			}
			scheme.reScaleByAndEqual(cipher, logp);		
		}
		delete[] gsrot;
		delete[] bsrot;
	} else {
		for(long i = 1; i < logrSlots; i++) {
			long gap = slots / (radix * pow(radix, logrSlots-i));
			cipher = scheme.rightRotateFast(cipher, (radix - 1) * gap);
			tmpCipher = cipher;
			scheme.multByPolyAndEqual(cipher, encodeV0poly[logrSlots-i][0], logp);
			for(long j = 1; j < (2 * radix - 1); j++) {
				Ciphertext rot = scheme.leftRotateFast(tmpCipher, j * gap);
				scheme.multByPolyAndEqual(rot, encodeV0poly[logrSlots-i][j], logp);
				scheme.addAndEqual(cipher, rot);
			}
			scheme.reScaleByAndEqual(cipher, logp);
		}
	}

	// last step //
	if(radix > 4) {
		// In this case, we use basy-step giant-step method
		bs = 1 << (long)floor((log2(radix)) / 2.);
		gs = (radix) / bs;
		Ciphertext* bsrotFirst = new Ciphertext[bs];
		Ciphertext* gsrotFirst = new Ciphertext[gs];		
		bsrotFirst[0] = cipher;

		NTL_EXEC_RANGE(bs-1,first,last)

		for(long i = first; i < last; ++i) { // baby-step
			long gap = slots / radix;
			bsrotFirst[i+1] = scheme.leftRotateFast(cipher, (i+1) * gap);
		}

		NTL_EXEC_RANGE_END

		NTL_EXEC_RANGE(gs,first,last)

		for(long i = first; i < last; ++i) {
			long gap = slots / radix;
			gsrotFirst[i] = scheme.multByPoly(bsrotFirst[0], encodeV0poly[0][i*bs], logp);
			for(long j = 1; j < bs; ++j) {
				Ciphertext tmp = scheme.multByPoly(bsrotFirst[j], encodeV0poly[0][i*bs+j], logp);
				scheme.addAndEqual(gsrotFirst[i], tmp);
			}
		}

		NTL_EXEC_RANGE_END

		cipher = gsrotFirst[0];
		for(long i = 1; i < gs; ++i) { // giant-step
			long gap = slots / radix;
			Ciphertext rot = scheme.leftRotateFast(gsrotFirst[i], i*bs*gap);
			scheme.addAndEqual(cipher, rot);
		}
		delete[] bsrotFirst;
		delete[] gsrotFirst;
	} else {
		tmpCipher = cipher;
		scheme.multByPolyAndEqual(cipher, encodeV0poly[0][0], logp);
		for(long i = 1; i < radix; ++i) {
			long gap = slots / radix;
			Ciphertext rot = scheme.leftRotateFast(tmpCipher, i * gap);
			scheme.multByPolyAndEqual(rot, encodeV0poly[0][i], logp);
			scheme.addAndEqual(cipher, rot);
		}
	}
	scheme.reScaleByAndEqual(cipher, logp);
	return;
}

void BootHelper::coeffToSlot(Ciphertext& part1, Ciphertext& part2, Ciphertext& cipher) {
	Ciphertext tmp1 = cipher;
	invV0(tmp1);
	Ciphertext tmp2 = scheme.conjugate(tmp1);
	part1 = scheme.add(tmp1, tmp2);
	part2 = scheme.sub(tmp1, tmp2);
	scheme.imultAndEqual(part2);
	scheme.negateAndEqual(part2);
	scheme.divByPo2AndEqual(part1, 1);
	scheme.divByPo2AndEqual(part2, 1);
	return;
}

void BootHelper::slotToCoeff(Ciphertext& cipher, Ciphertext& part1, Ciphertext& part2) {
	scheme.imultAndEqual(part2);
	cipher = scheme.add(part1, part2);
	V0(cipher);
	return;
}

/*
This part can be improved using Chebychev's method (or mini-max poly).
*/
void BootHelper::evalExpAndEqual(Ciphertext& cipher, long logT, long logI, long logq) {
	scheme.imultAndEqual(cipher); // i * theta
	cipher.logp += logI;
	scheme.divByPo2AndEqual(cipher, logT); // i * theta / (2^(logT + logI))
	scheme.exp2piAndEqual(cipher, logq + logI); // exp(2pi*i * theta / (2^(logT + logI)))
	for (long i = 0; i < logI + logT; ++i) {
		scheme.squareAndEqual(cipher);
		scheme.reScaleByAndEqual(cipher, logq + logI);
	}
	Ciphertext tmp1 = scheme.conjugate(cipher);
	scheme.subAndEqual(cipher, tmp1);
	scheme.imultAndEqual(cipher);
	scheme.negateAndEqual(cipher); // 2 * sin(2pi * theta)
	RR c = 0.25 / to_RR(M_PI);
	scheme.multByConstAndEqual(cipher, c, logq + logI); // 1/2pi * (sin(2pi * theta))
	scheme.reScaleByAndEqual(cipher, logq + 2 * logI);
}

void BootHelper::evalExpAllAndEqual(Ciphertext& part1, Ciphertext& part2, long logT, long logI, long logq) {
	scheme.imultAndEqual(part1); // i * theta
	scheme.imultAndEqual(part2); // i * theta
	part1.logp += logI; part2.logp += logI;
	scheme.divByPo2AndEqual(part1, logT); // i * theta / (2^(logT + logI))
	scheme.divByPo2AndEqual(part2, logT); // i * theta / (2^(logT + logI))
	scheme.exp2piAndEqual(part1, logq + logI); // exp(2pi*i * theta / (2^(logT + logI)))
	scheme.exp2piAndEqual(part2, logq + logI); // exp(2pi*i * theta / (2^(logT + logI)))
	for (long i = 0; i < logI + logT; ++i) {
		scheme.squareAndEqual(part1);
		scheme.squareAndEqual(part2);
		scheme.reScaleByAndEqual(part1, logq + logI);
		scheme.reScaleByAndEqual(part2, logq + logI);
	} // exp(2pi * i * theta) = cos(2pi * theta) + sin(2pi * theta) * i
	Ciphertext tmp1 = scheme.conjugate(part1);
	Ciphertext tmp2 = scheme.conjugate(part2);
	scheme.subAndEqual(part1, tmp1);
	scheme.subAndEqual(part2, tmp2);
	scheme.imultAndEqual(part1);
	scheme.imultAndEqual(part2);
	scheme.negateAndEqual(part1); // 2 * sin(2pi * theta)
	scheme.negateAndEqual(part2); // 2 * sin(2pi * theta)
	RR c = 0.25 / to_RR(M_PI);
	scheme.multByConstAndEqual(part1, c, logq + logI); // 1/2pi * (sin(2pi * theta))
	scheme.multByConstAndEqual(part2, c, logq + logI); // 1/2pi * (sin(2pi * theta))
	scheme.reScaleByAndEqual(part1, logq + 2 * logI);
	scheme.reScaleByAndEqual(part2, logq + 2 * logI);
	return;
}

void BootHelper::bootstrapping(Ciphertext& cipher, long logq, long logQ, long logT, long logI) {
	long logSlots = log2(cipher.n);
	long logp = cipher.logp;

	scheme.modDownToAndEqual(cipher, logq);
	scheme.normalizeAndEqual(cipher);

	TimeUtils time;

	cipher.logq = logQ;
	cipher.logp = logq;

	for (long i = logSlots; i < ring.logNh; ++i) {
		Ciphertext rot = scheme.leftRotateFast(cipher, (1 << i));
		scheme.addAndEqual(cipher, rot);
	}
	scheme.divByPo2AndEqual(cipher, ring.logNh - logSlots);
	
	Ciphertext part1, part2;

	// time.start("CoeffToSlot");
	coeffToSlot(part1, part2, cipher);
	// time.stop("CoeffToSlot");

	// time.start("Evaluate Exp");
	evalExpAndEqual(part1, logT, logI, logq);
	evalExpAndEqual(part2, logT, logI, logq);
	// time.stop("Evaluate Exp");

	// time.start("SlotToCoeff");
	slotToCoeff(cipher, part1, part2);
	// time.stop("SlotToCoeff");

	cipher.logp = logp;
}



void BootHelper::bootstrapping_cos(Ciphertext& cipher, long logq, long logQ, long logK) {
	long logSlots = log2(cipher.n);
	long logp = cipher.logp;

	scheme.modDownToAndEqual(cipher, logq);
	scheme.normalizeAndEqual(cipher);

	// TimeUtils time;

	cipher.logq = logQ;
	cipher.logp = logq;

	for (long i = logSlots; i < ring.logNh; ++i) {
		Ciphertext rot = scheme.leftRotateFast(cipher, (1 << i));
		scheme.addAndEqual(cipher, rot);
	}
	scheme.divByPo2AndEqual(cipher, ring.logNh - logSlots);
	
	Ciphertext part1, part2;

	// time.start("CoeffToSlot");
	coeffToSlot(part1, part2, cipher);
	// time.stop("CoeffToSlot");

	// time.start("Evaluate Sin * 2");
	evalSin2piAndEqual(part1, logK, logq);
	evalSin2piAndEqual(part2, logK, logq);
	// time.stop("Evaluate Sin * 2");

	// time.start("SlotToCoeff");
	slotToCoeff(cipher, part1, part2);
	// time.stop("SlotToCoeff");

	cipher.logp = logp;
}

void BootHelper::bootstrapping_cosDec(Ciphertext& cipher, long logq, long logQ, long logK, SecretKey sk) {
	long logSlots = log2(cipher.n);
	long logp = cipher.logp;

	scheme.modDownToAndEqual(cipher, logq);
	scheme.normalizeAndEqual(cipher);

	TimeUtils time;

	cipher.logq = logQ;
	cipher.logp = logq;

	for (long i = logSlots; i < ring.logNh; ++i) {
		Ciphertext rot = scheme.leftRotateFast(cipher, (1 << i));
		scheme.addAndEqual(cipher, rot);
	}
	scheme.divByPo2AndEqual(cipher, ring.logNh - logSlots);
	
	Ciphertext part1, part2;

	// time.start("CoeffToSlot");
	coeffToSlot(part1, part2, cipher);
	// time.stop("CoeffToSlot");

	complex<double>* dvecPart1 = scheme.decrypt(sk, part1);
	complex<double>* dvecPart2 = scheme.decrypt(sk, part2);

	// time.start("Evaluate Sin * 2");
	evalSin2piAndEqualDec(part1, logK, logq, sk);
	evalSin2piAndEqualDec(part2, logK, logq, sk);
	// time.stop("Evaluate Sin * 2");

	complex<double>* dvecPart1a = scheme.decrypt(sk, part1);
	complex<double>* dvecPart2a = scheme.decrypt(sk, part2);
	
	cout << "Part1 : before // after" << endl;
	for(int i = 0; i < cipher.n; i++) {
        std::cout << i << " : " << dvecPart1[i].real() << " // " << dvecPart1a[i].real() << " -- " << dvecPart1[i].real() - dvecPart1a[i].real() << std::endl;
    }
	
	cout << "Part2 : before // after" << endl;
	for(int i = 0; i < cipher.n; i++) {
        std::cout << i << " : " << dvecPart2[i].real() << " // " << dvecPart2a[i].real() << " -- " << dvecPart2[i].real() - dvecPart2a[i].real() << std::endl;
    }

	// time.start("SlotToCoeff");
	slotToCoeff(cipher, part1, part2);
	// time.stop("SlotToCoeff");

	cipher.logp = logp;
}

void BootHelper::evalSin2piAndEqual(Ciphertext& cipher, long logK, long logq) {
	// cout << "logq = " << cipher.logq << endl;
	
	scheme.addConstAndEqual(cipher, -0.25, logq); // x -> x - 1/4
	// cipher.logp += logI;
	//cout << " ======== cipher.logp = " << cipher.logp << endl;
	
	scheme.divByPo2AndEqual(cipher, logK); // x - 1/4 -> (x - 1/4) / K

	// cout << "logq = " << cipher.logq << endl;

	//* evaluate (x - 1/4) / K -> cos( (2pi * x - pi/2) / K)
	scheme.cos2piAndEqual(cipher, logq);
	// scheme.cos2piChebyAndEqual(cipher, logq);
	
	// cout << "logq = " << cipher.logq << endl;

	//* cos( (2pi * x - pi/2) / K) -> cos(2pi * x - pi/2) = sin(2pi * x)
	for(int i = 0; i < logK; i++) {
		scheme.squareAndEqual(cipher);
		scheme.reScaleByAndEqual(cipher, logq);
		scheme.addAndEqual(cipher, cipher);
		scheme.addConstAndEqual(cipher, -1.0, logq);
	}
	// cout << "logq = " << cipher.logq << endl;

	RR c = 0.5 / to_RR(M_PI);
	scheme.multByConstAndEqual(cipher, c, logq); // 1/2pi * (sin(2pi * x))
	scheme.reScaleByAndEqual(cipher, logq);

	// cout << "logq = " << cipher.logq << endl;
}


void BootHelper::evalSin2piAndEqualDec(Ciphertext& cipher, long logK, long logq, SecretKey sk) {
	// cout << "logq = " << cipher.logq << endl;
	
	scheme.addConstAndEqual(cipher, -0.25, logq); // x -> x - 1/4
	// cipher.logp += logI;
	//cout << " ======== cipher.logp = " << cipher.logp << endl;
	
	scheme.divByPo2AndEqual(cipher, logK); // x - 1/4 -> (x - 1/4) / K

	// cout << "logq = " << cipher.logq << endl;

	cout << "before cos" << endl;
	complex<double>* dvecB = scheme.decrypt(sk, cipher);
	for (int i = 0; i < cipher.n; i++)
	{
		cout << i << " : " << dvecB[i].real() << endl;
	}
	
	//* evaluate (x - 1/4) / K -> cos( (2pi * x - pi/2) / K)
	scheme.cos2piAndEqual(cipher, logq);
	// scheme.cos2piChebyAndEqual(cipher, logq);

	cout << "after cos" << endl;
	complex<double>* dvecA = scheme.decrypt(sk, cipher);
	for (int i = 0; i < cipher.n; i++)
	{
		cout << i << " : " << dvecA[i].real() << endl;
	}
	
	// cout << "logq = " << cipher.logq << endl;

	//* cos( (2pi * x - pi/2) / K) -> cos(2pi * x - pi/2) = sin(2pi * x)
	for(int i = 0; i < logK; i++) {
		scheme.squareAndEqual(cipher);
		scheme.reScaleByAndEqual(cipher, logq);
		scheme.addAndEqual(cipher, cipher);
		scheme.addConstAndEqual(cipher, -1.0, logq);
	}

	cout << "square" << endl;
	complex<double>* dvecS = scheme.decrypt(sk, cipher);
	for (int i = 0; i < cipher.n; i++)
	{
		cout << i << " : " << dvecS[i].real() << endl;
	}
	
	// cout << "logq = " << cipher.logq << endl;

	RR c = 0.5 / to_RR(M_PI);
	scheme.multByConstAndEqual(cipher, c, logq); // 1/2pi * (sin(2pi * x))
	scheme.reScaleByAndEqual(cipher, logq);

	cout << "div 2pi" << endl;
	complex<double>* dvecP = scheme.decrypt(sk, cipher);
	for (int i = 0; i < cipher.n; i++)
	{
		cout << i << " : " << dvecP[i].real() << endl;
	}

	// cout << "logq = " << cipher.logq << endl;
}

