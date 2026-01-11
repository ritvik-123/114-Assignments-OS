// Ritvik Mahapatra, 301894867
// run the program after saving this file in the same folder as source file, copy the source file content into data.in, then run the a.out file. Finally, copy the data.out into your required output file name with extension
#include <iostream>    // Input/Output stream
#include <fstream>     // File read/write
#include <unistd.h>    // Symbolic Constants
#include <sys/types.h> // Primitive System Data Types
#include <sys/wait.h>  // Wait for Process Termination
#include <cstdlib>     // General Utilities

int A_to_B[2]; // Pipe

int main() {
    int status;

    // Opening the input and output files
    std::ifstream inFile("data.in", std::ios::in | std::ios::binary);
    if (!inFile) {
        std::cerr << "Failed to open data.in" << std::endl;
        return EXIT_FAILURE;
    }

    std::ofstream outFile("data.out", std::ios::out | std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to open data.out" << std::endl;
        inFile.close();
        return EXIT_FAILURE;
    }

    // Creating the pipe
    if (pipe(A_to_B) == -1) {
        std::cerr << "Failed to create pipe" << std::endl;
        inFile.close();
        outFile.close();
        return EXIT_FAILURE;
    }

    if (fork() == 0) { // First child process (PA) - read from data.in and write to pipe
        double my_double;
        std::cout << "IN PA\n";
        close(A_to_B[0]); // Close read end of the pipe

        while (inFile.read(reinterpret_cast<char*>(&my_double), sizeof(double))) {
            if (write(A_to_B[1], &my_double, sizeof(double)) == -1) { // when writing fails or eof
                std::cerr << "Failed to write to pipe" << std::endl;
                inFile.close();
                close(A_to_B[1]);
                return EXIT_FAILURE;
            }
        }

        close(A_to_B[1]); // Close write end of the pipe to signal end of data
        inFile.close();
        return 0;
    }

    if (fork() == 0) { // Second child process (PB) - read from pipe and write to data.out
        double my_double;
        std::cout << "IN PB\n";
        close(A_to_B[1]); // Close write end of the pipe

        while (read(A_to_B[0], &my_double, sizeof(double)) > 0) {//till something remaining in pipe
            outFile.write(reinterpret_cast<char*>(&my_double), sizeof(double));
        }

        if (!outFile) {
            std::cerr << "Failed to write to data.out" << std::endl;
            close(A_to_B[0]);
            outFile.close();
            return EXIT_FAILURE;
        }

        close(A_to_B[0]); // Close read end of the pipe
        outFile.close();
        return 0;
    }

    // Parent process waits for both children to finish
    close(A_to_B[0]);
    close(A_to_B[1]);
    wait(&status);
    wait(&status);

    std::cout << "END\n";

    return 0;
}
