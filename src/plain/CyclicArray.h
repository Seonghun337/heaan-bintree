#ifndef CYCLICARRAY_H_
#define CYCLICARRAY_H_

#include <iostream>
using namespace std;

class CyclicArray {
public:
	long length;
	long start;
	long colNum;
	long rowNum;
	double* data;

	// --------------------
	// Initialize
	// --------------------
	CyclicArray() {}
	CyclicArray(double* array, long length_, long rowNum_, long colNum_);
	CyclicArray(double** array, long rowNum_, long colNum_);
	CyclicArray(double* array, long length_);
	CyclicArray(long length_);
	CyclicArray(CyclicArray& _ca);
	~CyclicArray() {}
	
	// --------------------

	/*
	 * leftRotate
	 * Output : CyclicArray a leftRotated by num
	 */
	void leftRotate(long num);
    void rightRotate(long num);

	/*
	 * setLength
	 * reDefine length of CyclicArray
	 */
	void setLength(long length_);

	/*
	 * get(index)
	 * Output : data[index]
	 */
	double get(long index) const;

    void set(long index, double data_) const;

    void add(CyclicArray given);
    void sub(CyclicArray given);
	void mult(CyclicArray given);
    
	/*
	 * Print CyclicArray
	 */
	void printAsVector() const;
	void printAsMatrix() const;

	// Create Random CyclicArray

	void randomGen(long length);

	// output data as a normal double*
	double* getArray();

};

/*
 * mult
 * result[i] = array1[i] * array2[i]
 */
void mult(CyclicArray& result, const CyclicArray& array1, const CyclicArray& array2);

/*
 * add
 * reslut[i] = array1[i] + array2[i]
 */
void add(CyclicArray& result, const CyclicArray& array1, const CyclicArray& array2);

void getMinMax(CyclicArray& caToMin, CyclicArray& caToMax);

#endif /* CYCLICARRAY_H_ */