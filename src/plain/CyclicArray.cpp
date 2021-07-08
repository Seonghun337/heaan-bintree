/*
 * CyclicArray.cpp
 *
 *  Created on: 2018. 6. 15.
 *      Author: swan
 */

#include "CyclicArray.h"

CyclicArray::CyclicArray(double* array, long length_, long rowNum_, long colNum_) {
	data = array;
	length = length_;
	start = 0;
	rowNum = rowNum_;
	colNum = colNum_;
}

CyclicArray::CyclicArray(double** array, long rowNum_, long colNum_) {
	length = rowNum_ * colNum_;
	rowNum = rowNum_;
	colNum = colNum_;
	start = 0;

	data = new double[length];
	for (int row = 0; row < rowNum; ++row) {
		for (int col = 0; col < colNum; ++col) {
			data[row * colNum + col] = array[row][col];
		}
	}
}

CyclicArray::CyclicArray(double* array, long length_) {
	length = length_;
	data = new double[length];
	for(int i = 0; i < length; i++) {
		data[i] = array[i];
	}
	start = 0;
	rowNum = 0;
	colNum = 0;
}

CyclicArray::CyclicArray(long length_) {
	length = length_;
	start = 0;
	rowNum = 0;
	colNum = 0;
	data = new double[length];
	for (int i = 0; i < length; ++i) {
		data[i] = 0;
	}
}
CyclicArray::CyclicArray(CyclicArray& _ca) {
	length = _ca.length;
	start = 0;
	rowNum = _ca.rowNum;
	colNum = _ca.colNum;
	data = new double[length];
	for (int i = 0; i < length; i++) {
		data[i] = _ca.get(i);
	}
	
}

double CyclicArray::get(long index) const {
	if (start + index < length) {
		return data[start + index];
	} else {
		return data[start + index - length];
	}
}

void CyclicArray::set(long index, double data_) const {
    if (start + index < length) {
		data[start + index] = data_;
	} else {
		data[start + index - length] = data_;
	}
}

void CyclicArray::setLength(long length_) {
	start = 0;
	length = length_;
	data = new double[length];
}

void CyclicArray::leftRotate(long num) {
	start += num;
	if (start >= length) {
		start -= length;
	}
}

void CyclicArray::rightRotate(long num) {
	start -= num;
	if (start < 0) {
		start += length;
	}
}

void CyclicArray::printAsVector() const {
	cout << "[";
	for (int i = 0; i < length; ++i) {
		cout << " " << get(i) << " ";
	}
	cout << "]" << endl;
}

void CyclicArray::printAsMatrix() const {
	for (int row = 0; row < rowNum; ++row) {
		cout << "{";
		for (int col = 0; col < colNum; ++col) {
			cout << " " << get(row * colNum + col) << " ";
		}
		cout << "}" << endl;
	}

}

void CyclicArray::add(CyclicArray given) {
    if (length != given.length) {
		cout << "CyclicArray::add Error, lengths are different" << endl;
		return;
	}

    for (long i = 0; i < length; ++i) {
		set(i, get(i) + given.get(i));
	}
}

void CyclicArray::sub(CyclicArray given) {
    if (length != given.length) {
		cout << "CyclicArray::add Error, lengths are different" << endl;
		return;
	}

    for (long i = 0; i < length; ++i) {
		set(i, get(i) - given.get(i));
	}
}

void CyclicArray::mult(CyclicArray given) {
    if (length != given.length) {
		cout << "CyclicArray::add Error, lengths are different" << endl;
		return;
	}

    for (long i = 0; i < length; ++i) {
		set(i, get(i) * given.get(i));
	}
}

void mult(CyclicArray& result, const CyclicArray& array1, const CyclicArray& array2) {
	if (array1.length != array2.length) {
		cout << "CyclicArray::mult Error, lengths are different" << endl;
		return;
	}
	result.setLength(array1.length);
	for (long i = 0; i < array1.length; ++i) {
        result.set(i, array1.get(i) * array2.get(i));
	}
	if(array1.rowNum==array2.rowNum) {
		result.rowNum=array1.rowNum;
	}
}

void add(CyclicArray& result, const CyclicArray& array1, const CyclicArray& array2) {
	if (array1.length != array2.length) {
		cout << "CyclicArray::add Error, lengths are different" << endl;
		return;
	}
	result.setLength(array1.length);
	for (long i = 0; i < array1.length; ++i) {
		// result.data[i] = array1.get(i) + array2.get(i);
        result.set(i, array1.get(i) + array2.get(i));
	}
	if(array1.rowNum==array2.rowNum) {
		result.rowNum=array1.rowNum;
	}
}

void getMinMax(CyclicArray& caToMin, CyclicArray& caToMax) {
    for(long i = 0; i < caToMin.length; i++) {
        double min = caToMin.get(i) < caToMax.get(i) ? caToMin.get(i) : caToMax.get(i);
        double max = caToMin.get(i) < caToMax.get(i) ? caToMax.get(i) : caToMin.get(i);
        caToMin.set(i, min);
        caToMax.set(i, max);
    }
}

double* CyclicArray::getArray() {
	double* ans = new double[length];
	for(int i = 0; i < length; i++) {
		ans[i] = get(i);
	}
	return ans;	
}



void CyclicArray::randomGen(long length_) {
    length = length_;
    start = 0;
    data = new double[length];
    for(int i = 0; i < length; i++) {
        data[i] = 0;
        data[i] = (double) rand() / RAND_MAX;
		data[i] = (double) rand() / RAND_MAX;
    }
}