// Ritvik Mahapatra, 301894867
// run the program after saving this file in the same folder as source file, copy the source file content into data.in, then run the a.out file. Finally, copy the data.out into your required output file name with extension
#include <iostream>
#include <fstream>
using namespace std;

int main()
{
    ifstream inFile;

    ofstream OutFile; // declaring input and output stream file variables

    double my_double; // declaring double

    inFile.open("data.in", ios::in | ios::binary);

    OutFile.open("data.out", ios::out | ios::binary); // opening data.in and data.out

    while(!inFile.eof())
    {
        inFile.read( (char*)&my_double, sizeof(double));

        OutFile.write( (char*)&my_double, sizeof(double));
    } // copying double by double the content of data.in into data.out

    inFile.close();

    OutFile.close(); // closing the files

    return 0;

}