#include <iostream>
#include <mpi.h>



int Global_sum(int rand_num, int my_rank, int numProc, MPI_Comm comm) {
    int        sum = rand_num;
    int        temp;
    int        partner;
    int        done = 0;
    unsigned   bitmask = 1;
    MPI_Status status;

    while (!done && bitmask < numProc) {
        partner = my_rank ^ bitmask;
        if (my_rank < partner) {
            MPI_Recv(&temp, 1, MPI_INT, partner, 0, comm, &status);
            sum = sum+temp;
            bitmask <<= 1;
        } else {
            MPI_Send(&sum, 1, MPI_INT, partner, 0, comm); 
            done = 1;
        }
    }
    return sum;
} 

int main(int argc, char* argv[]) {
    using namespace std;
    int      numProc, my_rank;
    int      rand_num;
    int      sum;
    MPI_Comm comm;
    MPI_Init(&argc, &argv);
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(MPI_COMM_WORLD, &numProc);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    srand(my_rank+1);
    rand_num = rand() % 100;
    cout << "Process Id:" << my_rank << " random number:" << rand_num << "\n";
    sum = Global_sum(rand_num, my_rank, numProc, comm);
    if (my_rank == 0)
        cout << "Global sum on process id 0 is: " << sum << "\n";
    MPI_Finalize();
    return 0;
} 

