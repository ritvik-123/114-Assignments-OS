// Ritvik Mahapatra, 301894867

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>
#include <cstdlib> // for atoi

using namespace std;

int M, N, K;
mutex mt;

int A[100][100], B[100][100], C[100][100]; // Matrix C for result

void readIntoArray(string file_name, int (&array)[100][100], int m, int n) {
    ifstream inFile;
    inFile.open(file_name.c_str(), ios::in); // open file correctly
    
    if (!inFile) {
        cerr << "Unable to open file " << file_name << endl;
        exit(1); // exit program if file cannot be opened
    }

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            inFile >> array[i][j];
        }
    }

    inFile.close();
}

int threadCallback(int x, int y) {
    mt.lock();
    cout << "Cell " << x << "," << y << endl;
    mt.unlock();

    int sum = 0;
    for (int i = 0; i < N; ++i) {
        sum += A[x][i] * B[i][y];
    }

    C[x][y] = sum; // store result in matrix C

    return sum;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "insufficient arguments to run" << endl;
        exit(1);
    }

    M = atoi(argv[1]);
    N = atoi(argv[2]);
    K = atoi(argv[3]);

    readIntoArray("A.txt", A, M, N);
    readIntoArray("B.txt", B, N, K);

    thread myThreads[M][K];

    // Create threads to compute matrix multiplication
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < K; ++j) {
            myThreads[i][j] = thread(threadCallback, i, j);
        }
    }

    // Join threads
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < K; ++j) {
            myThreads[i][j].join();
        }
    }

    // Print matrix C
    cout << "Matrix C:" << endl;
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < K; ++j) {
            cout << C[i][j] << " ";
        }
        cout << endl;
    }

    return 0;
}