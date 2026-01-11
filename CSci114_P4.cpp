// Ritvik, Assignment-4, 301894867
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std;

// Shared resources and synchronization primitives
vector<int> available; // Available resources
vector<vector<int>> maxDemand, allocation, need; // Max demand, allocation, and need matrices
mutex mtx; // Mutex for synchronization
condition_variable cv; // Condition variable for resource allocation
int numThreads; // Number of threads (processes)
int capacity; // Resource capacity

// Print the allocation matrix for debugging
void printAllocation() {
    cout << "[ ";
    for (int i = 0; i < numThreads; ++i) {
        cout << allocation[i][0] << " ";
    }
    cout << "]" << endl;
}

// Check if the system is in a safe state
bool isSafe() {
    vector<int> work = available;
    vector<bool> finish(numThreads, false);
    vector<int> safeSequence;

    // Try to find a safe sequence
    while (safeSequence.size() < numThreads) {
        bool found = false;
        for (int i = 0; i < numThreads; ++i) {
            if (!finish[i] && need[i][0] <= work[0]) {
                // Simulate resource allocation
                work[0] += allocation[i][0];
                finish[i] = true;
                safeSequence.push_back(i);
                found = true;
            }
        }
        if (!found) return false; // No safe sequence found
    }
    return true; // Safe sequence found
}

// Request resources for a thread
bool requestResources(int threadID, int request) {
    unique_lock<mutex> lock(mtx);

    // Wait if the requested resources are not available
    while (request > available[0]) {
        cv.wait(lock);
    }

    // Tentatively allocate resources
    available[0] -= request;
    allocation[threadID][0] += request;
    need[threadID][0] -= request;

    // Check if the system is still in a safe state
    if (!isSafe()) {
        // Rollback allocation
        available[0] += request;
        allocation[threadID][0] -= request;
        need[threadID][0] += request;
        printAllocation();
        return false; // Request cannot be satisfied
    }

    printAllocation();
    return true; // Request successfully satisfied
}

// Release resources for a thread
void releaseResources(int threadID, int release) {
    unique_lock<mutex> lock(mtx);

    // Release resources and update matrices
    available[0] += release;
    allocation[threadID][0] -= release;
    need[threadID][0] += release;

    printAllocation();
    cv.notify_all(); // Notify all waiting threads
}

// Thread function to process requests
void threadFunction(int threadID) {
    ifstream file("requests-2.txt");
    if (!file.is_open()) {
        cerr << "Error opening requests-2.txt" << endl;
        return;
    }
    string line;
    
    // Process each line in the request file
    while (getline(file, line)) {
        istringstream iss(line);
        int id, units;
        iss >> id >> units;

        if (id == threadID) {
            // Request resources
            for (int i = 0; i < units; ++i) {
                while (!requestResources(threadID, 1)) {
                    this_thread::sleep_for(chrono::milliseconds(100)); // Wait before retrying
                }
            }
            // Release resources
            releaseResources(threadID, units);
        }
    }
}

// Main function to initialize matrices and start threads
int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " N M" << endl;
        return 1;
    }

    numThreads = stoi(argv[1]);
    capacity = stoi(argv[2]);

    // Validate input
    if (numThreads < 5 || numThreads > 15 || capacity < 5 || capacity > 10) {
        cerr << "N must be between 5 and 15, M must be between 5 and 10." << endl;
        return 1;
    }

    // Initialize matrices
    available = vector<int>(1, capacity);
    maxDemand = vector<vector<int>>(numThreads, vector<int>(1, capacity));
    allocation = vector<vector<int>>(numThreads, vector<int>(1, 0));
    need = vector<vector<int>>(numThreads, vector<int>(1, capacity));

    // Create and start threads
    vector<thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(threadFunction, i);
    }

    // Join threads
    for (auto& th : threads) {
        th.join();
    }

    return 0;
}