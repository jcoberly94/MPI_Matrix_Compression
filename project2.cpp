//docker run --rm -it -v $(pwd):/project nlknguyen/alpine-mpich
//mpic++ -std=c++11 project2.cpp -o project 2
//mpirun -np 3 ./project2 test1.txt 3 4
#include <time.h>
#include <stdlib.h>
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <iomanip>

using namespace std;

int main(int argc, char** argvs) {

    int rank = 0;
    int size = 0;

    MPI_Init(&argc, &argvs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

	//Error Checking
    if (size <= 1)
	{
		cout << "Assign at least 2 processes and retry" << endl;
		MPI_Finalize();
		return -1;
	}
	
	//MASTER PROCESS
    if (rank == 0)	
	{
		list<string> data;
		string fileName = argvs[1];
		int totalRows = atoi(argvs[2]);
		int totalCols = atoi(argvs[3]);	
		cout << "Total Rows: " << totalRows << endl;
		cout << "Total Columns: " << totalCols << endl;


		int matrix[totalRows][totalCols] = {0};
		

		//READ FILE
		fstream read;

		read.open(fileName);

		for (int x = 0; x <= totalRows; x++)
		{
			for (int y = 0; y < totalCols; y++)
			{
				read >> matrix[x][y];
			}
		}


		read.close();
		//--------------------------------------------------//

		//Main Process Function
		int worker = 0;
		//Take the matrix and create an array for each row, passing it off to the child processes
		for(int i = 0; i < totalRows; i++) {
			int listcolumns = totalCols*2;
			int resultData[listcolumns];
			int row[totalCols];
			int returnDataSize = listcolumns;
			memcpy(row, matrix[i], sizeof(row));

			
			//if # of rows exceeds processors, this will keep the program rotating through the available ones
			int process = (worker % (size-1)) + 1;

			//sending the array to the processor
			MPI_Send(row, totalCols, MPI_INT, process, process, MPI_COMM_WORLD);
			cout << "sending out worker: " << worker+1 << endl;
			cout << "process is marked: " << process << endl;
			MPI_Recv(&resultData, returnDataSize, MPI_INT, process, process, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			//receive the processed data, trim off fixed size and store it into linked list
			for( int j = 0; j < listcolumns; j+=2) {				
				if(resultData[j] == 0) { break; }
				else {
					string answer = "Val: " + to_string(resultData[j]) + " row,col: (" + to_string(worker+1) + ", "+ to_string(resultData[j+1]) + ")";
					data.push_back(answer);
				}
			}
			cout << endl;
			//keep track of workers, should not exceed the number of rows
			worker++;
			
		}
		//display data
		for (auto v : data) {
			cout << v << endl;
		 }


	//CHILD PROCESS------------------------------------------------/
	} else if ( rank < size && rank >= 1 ) {
		
		//get necessary information and create an array that is twice the size of the colums to store position
		int length;		
		int workers = size-1;
		int rows = (atoi(argvs[2]));
		int cols = (atoi(argvs[3]));
		int dataChunk[cols];
		int listcolumns = cols*2;
		int returnData[listcolumns] = { 0 };
		int counter = 0;		

		//Keep listening for new data coming in to process
		for( int a = rank; a <= rows; a += workers) { 
			MPI_Recv(dataChunk, length, MPI_INT, 0, rank, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

			//loop over chunk of data and remove only store non 0s
			for( int i = 0; i < cols-1; i++) {
				if(dataChunk[i] > 0) {
					
					returnData[counter] = dataChunk[i];
					counter++;
					returnData[counter] = i + 1;
					counter++;
					
				}
				
			}
			MPI_Send(&returnData, listcolumns, MPI_INT, 0, rank, MPI_COMM_WORLD);

			//reset information for next round of incoming data
			counter = 0;
			std::fill_n(returnData, listcolumns, 0);
		
		}

	}
    
    MPI_Finalize();

    return 0;
}