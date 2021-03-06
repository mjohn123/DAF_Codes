#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include "MersenneTwister.h"

#include <string>//param in ReadFileDouble
#include <vector>

//#ifndef DEBUG_BATS
//#define DEBUG_BATS
//#endif

using namespace std;


#define SYMBOLSIZE 8
#define MAX_DEGREE 2000

typedef unsigned char SymbolType;
typedef unsigned short KeyType;
typedef unsigned long uint32;
typedef unsigned short uint16;
// for delay-aware
typedef unsigned long PosType;
typedef float ModeType;
//typedef vector<SymbolType> SymVector;


//inline void getCodeParam(const int & packetNum, int & LDNum, int & HDNum, int & piNum, int & smNum) {
//    LDNum = (int) (BATS_PRECODE_RATE * packetNum);
//
//    if (LDNum > 0) {
//        HDNum = 10;
//        HDNum = 0; // for testing only
//
//    } else { // BATS_PRECODE_RATE == 0 for no precoding
//        HDNum = 0;
//    }
//
//    piNum = 0;
//    smNum = packetNum + LDNum + HDNum - piNum;
//}

inline void saveIDInPacket(SymbolType* packet, KeyType* id){
//    SymbolType* a = (SymbolType*)id;
//    for(int i = 0; i< sizeof(KeyType); i++){
//        packet[i] = a[i];
//    }
    memcpy(packet, (SymbolType*) id, sizeof(KeyType));
}

inline KeyType getIDFromPacket(SymbolType* packet){
    KeyType id;
//    SymbolType* a = (SymbolType*)&id;
//    for(int i = 0; i< sizeof(KeyType); i++){
//        a[i] = packet[i];
//    }
    memcpy((SymbolType*)&id, packet, sizeof(KeyType));
    return id;
}

class DistSampler {
    double* accu;
    int n;
public:
    DistSampler(double* dist, int D):n(D){
        double sum = 0.0;

        accu = (double*)malloc((D+1)*sizeof(double));
        
        accu[0] = 0.0;

        for(int i = 0; i<D; i++){
            sum += dist[i];
            accu[i+1] = accu[i] + dist[i];
        }
    }

    inline int sample(double pos){ // pos in [0 1]
        int lb = 0, ub = n, mi;
		while(ub-lb > 1){
			mi = lb+(ub-lb)/2;
			if(pos < accu[mi])
				ub = mi;
			else
				lb = mi;
		}
		return lb;
    }

    ~DistSampler(){
        if (accu != NULL)
            free(accu);
    }

    int getMaxDegree(){
        return n;
    }
};

//inline void ShowRMatrix(const vector<SymVector>& M){
//    int nRow = M.size();
//    int nCol = M[0].size();
//    for(int i = 0; i < nRow; i++){
//        for(int j=0; j< nCol; j++){
//            printf("%d  ", M[i][j]);
//        }
//        printf("\n");
//    }
//}
//
//inline void ShowCMatrix(const vector<SymVector>& M){
//    int nCol = M.size();
//    int nRow = M[0].size();
//    for(int i = 0; i < nRow; i++){
//        for(int j=0; j< nCol; j++){
//            printf("%d  ", M[j][i]);
//        }
//        printf("\n");
//    }
//}
//
//inline vector<SymVector> MrToMc(const vector<SymVector>& M){
//
//    int nRow = M.size();
//    int nCol = M[0].size();
//
//    vector<SymVector> r(nCol,SymVector(nRow,0));
//
//    for(int i = 0; i < nRow; i++){
//        for(int j=0; j< nCol; j++){
//            r[j][i] = M[i][j];
//        }
//    }
//    return r;
//}

template<class T>
inline T** mallocMat(int row, int col){
    T** p = (T**)malloc(row*sizeof(T*));
    for(int i = 0 ; i<row; i++){
        p[i] = (T*)malloc(col*sizeof(T));
        memset(p[i],0,col*sizeof(T));
    }
    return p;
}

template<class T>
inline void freeMat(T** p, int row){
    for(int i = 0 ; i<row; i++){
        free(p[i]);
    }
    free(p);
}

//Tom: Added on 4 July, 2013.
template<class T>
inline T** newIdentityMat(int n){
	T** p = (T**)malloc(n*sizeof(T*));
	for (int i = 0; i < n; i++) {
		p[i] = (T*)malloc(n*sizeof(T));
		memset(p[i], 0, n*sizeof(T));
		p[i][i] = 1;
	}
	return p;
}
template<class T>
inline void setIdentityMatrix(T** p, int n){
	for (int i = 0; i < n; i++) {
		memset(p[i], 0, n*sizeof(T));
		p[i][i] = 1;
	}
}
//--

template<class T>
class ArrayQueue {
private:
    T* data;
    int head;
    int tail;
    int bufsize;

public:
    ArrayQueue(int s):bufsize(s){
        data = new T[bufsize];
        tail = 0;
        head = 0;
    }
    ~ArrayQueue(){
        delete [] data;
    }

    inline int size() {
        return (tail-head);
    }
    inline void empty(){
        head = 0;
        tail = 0;
    }

    inline bool isNonEmpty(){
        return head < tail;
    }

    inline void push(T x){
        data[(tail++)%bufsize] = x;
    }

    inline T pop(){
        return data[(head++)%bufsize];
    }
};

//class BMatrix {
//private:
//    bool transposed;
//    vector<int> data;
//public:
//    int row, col;
//    BMatrix(int row, int col):row(row), col(col), transposed(0), data(row*col,0){}
//    BMatrix(int row, int col, vector<int> data):row(row), col(col), transposed(0), data(data){}
//
//    inline int& operator()(int a, int b){
//        return transposed? data[a+b*col] : data[a*row+b];
//    }
//
//    inline const int& operator()(int a, int b) const {
//        return (transposed? data[a+b*col] : data[a*row+b]);
//    }
//
//    inline void transpose(){
//        transposed = !transposed;
//        swap(row,col);
//    }
//};


class WrongDegree{
public:
    WrongDegree(){}
    WrongDegree(int theDegree): degree(theDegree){}
    int getDegree() const {return degree;}
private:
    int degree;
};

//template <class C> void FreeClear( C & cntr ) {
//    for ( typename C::iterator it = cntr.begin();
//              it != cntr.end(); ++it ) {
//        delete * it;
//    }
//    cntr.clear();
//}

template <class T>
void printMat(T* a, int row, int col){
    for (int i = 0 ; i< row; i++) {
        for (int j = 0; j< col; j++)
            cout << (int)(a[i*col + j]) << " ";

        cout << endl;
    }
}

//    double degreeDist[500];
//    int D = 200;
//    double sum = 0;
//    int r = batchSize-1;
//    
//    for(int i = 0; i < 500; i++){
//        if (i < r){
//            degreeDist[i] = 0;
//        } else if (i == r) {
//            degreeDist[i] = 0.05;
//        } else if (i < D){
//            degreeDist[i] = r/(double)i/(double)(i-1);
//        } else if (i == D){
//            degreeDist[i] = r/(double)(D-1);
//        } else {
//            degreeDist[i] = 0;
//        }
//        sum += degreeDist[i];
//    }
//    
//    for(int i = r; i <= D; i++){
//        degreeDist[i] = degreeDist[i]/sum;
//        if (degreeDist[i]>1e-9)
//            cout << "p[" << i << "] = " << degreeDist[i] << endl;
//    }


//Tom: added 31 July, 2013.
inline vector<double>* ReadFileDouble(const string filename) {
	vector<double>* content = new vector<double>();
	ifstream file;
	file.open(filename.c_str());
	if (!file) {
		cout << "Cannot open file: " << filename << endl;
	}
	
	double x;
	while (file >> x) {
		content->push_back(x);
	}
	
	file.close();
	return content;
}

#endif /* UTILITIES_H_ */

