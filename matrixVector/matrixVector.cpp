#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[]) {
	using namespace std;
	int mpiErr = MPI_Init(&argc, &argv);
    int	Numprocs, MyRank;
	mpiErr = MPI_Comm_rank(MPI_COMM_WORLD, &MyRank);
	mpiErr = MPI_Comm_size(MPI_COMM_WORLD, &Numprocs);

    int 	    NoofCols, NoofRows, VectorSize; 
    int	    index, irow, icol, iproc, ValidInput=1;
    int	    Root=0, Source, Destination;
    int	    Source_tag, Destination_tag;
    int	    RowtoSend = 0;
    float     **Matrix, *Buffer, Sum;
    float     *Vector, *FinalVector;
    float     *CheckResultVector;
    FILE	    *fp;
    int	    MatrixFileStatus = 1, VectorFileStatus = 1;
    MPI_Status status;   

    if(MyRank == 0) {
        if ((fp = fopen ("mdata.inp", "r")) == NULL){
            cout << "null ayo";
            MatrixFileStatus = 0;
        }

        if(MatrixFileStatus != 0) {
            fscanf(fp, "%d %d\n", &NoofRows,&NoofCols);
            cout << "NoofRows: " << NoofRows << "\n";
            cout << "NoofCols: " << NoofCols << "\n";
            /* ...Allocate memory and read Matrix from file .......*/
            Matrix = (float **)malloc(NoofRows*sizeof(float *));
            for(irow=0 ;irow<NoofRows; irow++){
                Matrix[irow] = (float *)malloc(NoofCols*sizeof(float));
                for(icol=0; icol<NoofCols; icol++) {
                    fscanf(fp, "%f", &Matrix[irow][icol]);     
                    }
            }
            fclose(fp);
            /* .......Convert 2-D Matrix into 1-D Array .....*/
            Buffer = (float *)malloc(NoofRows*NoofCols*sizeof(float));

            index = 0;
            for(irow=0; irow< NoofRows; irow++){
                for(icol=0; icol< NoofCols; icol++) {
                        Buffer[index] = Matrix[irow][icol]; 
                        index++;
                }
            }
        }

        /* Read vector from input file */
        if ((fp = fopen ("vdata.inp", "r")) == NULL){
            VectorFileStatus = 0;
        }
        if(VectorFileStatus != 0) {
            fscanf(fp, "%d\n", &VectorSize);
            cout << "VectorSize: " << VectorSize << "\n";
            Vector = (float*)malloc(VectorSize*sizeof(float));
            for(index = 0; index<VectorSize; index++)
                fscanf(fp, "%f", &Vector[index]);     
        }
        cout << "Vector: " << Vector[5] << "\n";
    } 

    if(MyRank != 0) 
        Vector = (float *)malloc(VectorSize*sizeof(float));

    /* .......Allocate memory for output and a row of a matrix......*/ 
    Buffer= (float *)malloc(NoofCols*sizeof(float));
    FinalVector = (float *)malloc(NoofRows*sizeof(float)); 
    for(irow=0; irow< NoofRows; irow++) 
        FinalVector[irow] = 0;

    /* Send At Data to each processors */
    for(irow = 1 ; irow < Numprocs ; irow++){
        for(icol=0; icol< NoofCols; icol++){	 
            Buffer[icol] = Matrix[irow-1][icol];
        }
        MPI_Send( Buffer, NoofCols, MPI_FLOAT, irow, RowtoSend+1, 
        MPI_COMM_WORLD);
        RowtoSend++;
    }

    for(irow = 0 ; irow < NoofRows ; irow++){
        MPI_Recv(&Sum, 1, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, 
            MPI_COMM_WORLD, &status);
        Destination = status.MPI_SOURCE;
        Source_tag  = status.MPI_TAG;
        FinalVector[Source_tag] = Sum;

        if( RowtoSend < NoofRows ){
        Destination_tag = RowtoSend+1;
        for(icol=0; icol< NoofCols; icol++)
            Buffer[icol] = Matrix[RowtoSend][icol];

        MPI_Send(Buffer, NoofCols, MPI_FLOAT, Destination, 
            Destination_tag, MPI_COMM_WORLD);
        RowtoSend++;
        }    
    }

    for(iproc = 1 ; iproc < Numprocs ; iproc++) 
        MPI_Send(Buffer, NoofCols, MPI_FLOAT, iproc, 0, MPI_COMM_WORLD); 

    for(irow = 0 ; irow < NoofRows ; irow++) 
        cout << "Final Vector: [" <<irow <<"]: "<<FinalVector[irow];
    

	mpiErr = MPI_Finalize();
    return 0;
}