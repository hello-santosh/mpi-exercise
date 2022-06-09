#include <iostream>
#include <mpi.h>

// A simple MPI program where even processess will send the message and odd processes will receive them.

int main(int argc, char* argv[]) {
	using namespace std;
	int mpiErr = MPI_Init(&argc, &argv);
	int procId, numProc;
	mpiErr = MPI_Comm_rank(MPI_COMM_WORLD, &procId);
	mpiErr = MPI_Comm_size(MPI_COMM_WORLD, &numProc);

	// if (numProc % 2 != 0) {
	// 	cout << "EROOR: Number of processes should be even! \n";
	// 	return MPI_Abort(MPI_COMM_WORLD, 1);
	// }

	// if (procId % 2 == 0) {
	// 	double msg = 555;
	// 	MPI_Send(&msg, 1, MPI_DOUBLE, procId + 1, 0, MPI_COMM_WORLD);
	// 	cout << "Process Id: " << procId << " sent message: " << msg << " to " << procId + 1 << "\n";
	// }
	// else {
	// 	double msg;
	// 	MPI_Status status;
	// 	mpiErr = MPI_Recv(&msg, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
	// 	if (mpiErr == MPI_SUCCESS)
	// 		cout << "Process Id: " << procId << " received message: " << msg << "\n";
	// 	else
	// 		cout << "Process Id: " << procId << " didnot receive any message." << "\n";
	// }

	//MPI Broadcast
	int number_of_tosses = 0;
    int number_in_circle = 0;
    double x,y,z,pi_estimate;
    int local_sum;
	if (procId == 0) {
		number_of_tosses = 1000000;
	    MPI_Bcast(&number_of_tosses, 1, MPI_INT, 0, MPI_COMM_WORLD);
	}
    for (int toss = 0; toss < number_of_tosses; toss++) {
        x = (double)drand48();					
		y = (double)drand48();	
        z = ((x*x) + (y*y));
        if (z <= 1) number_in_circle++;
    }
    MPI_Reduce(&number_in_circle, &local_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	if (procId == 0) {
        pi_estimate = 4*local_sum/((double) number_of_tosses);
        cout << "Pi Estimate: " << pi_estimate << "\n";
	}





	//cout << "Data before Broadcast in Process" << procId << " is " << val << "\n";
	//MPI_Bcast(&val, 1, MPI_INT, 0, MPI_COMM_WORLD);
	//cout << "Data after Broadcast in Process" << procId << " is " << val << "\n";

	//MPI Scatter
	//int* buff = NULL;
	//if (procId == 0) {
	//	int arr[10] = { 1,2,3,4,5,6,7,8,9,10 };
	//	buff = arr;
	//}
	//cout << "Data before scatter in Process" << procId << " is " << val << "\n";
	//MPI_Scatter(buff, 1, MPI_INT, &val, 1, MPI_INT, 0, MPI_COMM_WORLD);
	//cout << "Data after scatter in Process" << procId << " is " << val << "\n";

	//MPI Gather
	//int arr[4], myVal;
	//myVal = procId * 2;
	//MPI_Gather(&myVal, 1, MPI_INT, arr, 1, MPI_INT, 0, MPI_COMM_WORLD);
	//if (procId == 0)
	//	for (int i = 0; i < 4; i++)
	//		cout << i << "th array value: " << arr[i] << "\n";

	//MPI Reduce
	//int data = 0;
	//int redData = 0;
	//data = procId;
	//if (procId == 0)
	//	cout << "Before Reduce call, data: " << redData << "\n";
	//MPI_Reduce(&data, &redData, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	//if (procId == 0) 
	//	cout << "After Reduce call, data: " << redData << "\n";

	mpiErr = MPI_Finalize();
}