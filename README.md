# MPI_Matrix_Compression

Just a simple project using MPI that reads in a text file and given matrix row and column size. Designed to handle large sparse matrix files (filled with lots of 0s) and compress them down to linked lists that give values, row, and position data on non zero indexes.


# Author: Justin Coberly
		
To compile: mpic++ -std=c++11 project2.cpp -o project2

To execute: mpirun -np (numOfProcesses) project2 (fileName).txt (numOfRows) (numOfCols)
Example: mpirun -np 6 project2 test1.txt 30 30

The number of rows and columns is specified in the input files. (do not exceed 30rows and 30columns in file1.txt as I do not have error handling for a larger input than the file)

Tested to work on Windows in Docker
