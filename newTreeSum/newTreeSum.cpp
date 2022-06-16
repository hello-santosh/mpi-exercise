#include <iostream>
#include <mpi.h>


int Global_sum(int my_rank, int size, int rand_num) {
    int remaining  = size;
    int sum = rand_num;
    int halfProc, temp;
    
    while(remaining != 1){
        halfProc = remaining/2;
        if(my_rank < halfProc){
            MPI_Status status;
            MPI_Recv(&temp, 1, MPI_INT, my_rank+halfProc, 0, MPI_COMM_WORLD, &status);
            sum+=temp;
        }else if(my_rank < remaining){
            MPI_Send(&sum, 1, MPI_INT, my_rank-halfProc, 0, MPI_COMM_WORLD);
            return 0;
        }
        remaining = halfProc;
    }

    if (my_rank == 0)
        std::cout << "Global sum on process id 0 is: " << sum << "\n";

} 

int main(int argc, char* argv[]) {
    using namespace std;
    int numProc, my_rank,rand_num,totalSum;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProc);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    srand(my_rank+1);
    rand_num = rand() % 100;
    cout << "\n Process Id:" << my_rank << " random number:" << rand_num << "Total process:  " << numProc << "\n";
    Global_sum(my_rank, numProc,rand_num);
    MPI_Finalize();
    return 0;
} 

