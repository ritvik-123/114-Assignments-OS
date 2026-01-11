# CSCI 114 — Operating Systems Assignments (C++)

A collection of CSCI 114 (Operating Systems) programming assignments implemented in C++.

## Assignments
- `CSci114_P1.cpp`  
  Demonstrates basic **binary file I/O** by reading `double` values from `data.in` and writing them to `data.out` (a simple binary copy program).

- `CSci114_P2.cpp`  
  Demonstrates **process creation and IPC** using `fork()` and a **pipe**: one child reads doubles from `data.in` and writes to the pipe, and another child reads from the pipe and writes to `data.out`.

- `CSci114_P3.cpp`  
  Implements **multi-threaded matrix multiplication** where each thread computes one cell of the result matrix `C` from matrices `A` and `B`.

- `CSci114_P4.cpp`  
  Simulates **resource allocation with deadlock avoidance** using a Banker's-algorithm style **safe-state check**; multiple threads request and release a shared resource while the program ensures the system remains safe.

- `CSci114_P5-1.cpp`  
  A first version of a **memory allocation simulator** using a free list and a **best-fit** style selection for allocating blocks, processing allocation/release commands from an input file.

- `CSci114_P5-Complete.cpp`  
  A complete **dynamic memory management simulator** with a free list + allocation list, using **best-fit allocation** and **merging freed blocks**, and writing final free-block sizes to `final_size.txt`.

## Build & Run (example)
```bash
g++ -std=c++17 CSci114_P3.cpp -o P3
./P3 <args-if-needed>
