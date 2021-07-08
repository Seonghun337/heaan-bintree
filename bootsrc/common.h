#ifndef COMMON_H_
#define COMMON_H_

// Standard Headers
#include <algorithm>
#include <math.h>

// Headers for the Eigen lib
#include <Eigen/Sparse>
#include <Eigen/Dense>

// Header for the HEAAN lib
#include "../HEAAN/src/HEAAN.h"

using namespace std;
using namespace Eigen;

typedef SparseMatrix<complex<double>> MatrixXc;

void conjugate(MatrixXc& A);

void divideBy(MatrixXc& A, double d);

double diff(complex<double>* a1, complex<double>* a2, long n);

uint32_t bitReverse(uint32_t x);

void bitReverseAndEqual(complex<double>* a, int n);

// this code is copied from "https://en.wikipedia.org/wiki/Cooley–Tukey_FFT_algorithm"
void separate(complex<double>* a, int n);

// this code is copied from "https://en.wikipedia.org/wiki/Cooley–Tukey_FFT_algorithm"
void FFT(complex<double>* x, int n);


#endif