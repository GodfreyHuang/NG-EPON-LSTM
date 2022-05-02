#pragma once
#include <algorithm>
#include <vector>
#include <iterator>

template<class T>
void InitVec(std::vector<T>& vec, int size) {
	for (int i = 0; i < size; i++) {
		T t;
		vec.push_back(t);
	}
}

template<class T>
void InitVecConstant(std::vector<T>& vec, int size, T constant) {
	for (int i = 0; i < size; i++) {
		vec.push_back(constant);
	}
}

template<class T>
void InitVecVec(std::vector< std::vector<T>>& vv, int inSize, int outSize) { // inSize: how many initial element you want in inner vector, outSize: for outer vector
	for (int i = 0; i < outSize; i++) {
		std::vector<T> vec;
		for (int j = 0; j < inSize; j++) {
			T t;
			vec.push_back(t);
		}
		vv.push_back(vec);
	}
}

template<class T>
void VecRemoveVal(std::vector<T>& vec, T val) {
	vec.erase(remove(vec.begin(), vec.end(), val), vec.end());
}

template<class T>
bool FindValInVec(std::vector<T>& vec, T val) {
    for(typename std::vector<T>::iterator it=vec.begin(); it!=vec.end(); it++){
        if(*it == val)
            return true;
    }
    return false;
}

template<class T>
int GetIndexOfValInVec(std::vector<T>& vec, T val) { // make sure you already confirmed that that value is really inside the vector
    for(typename std::vector<T>::iterator it=vec.begin(); it!=vec.end(); it++){
        if(*it == val)
            return std::distance(vec.begin(), it);
    }
}
