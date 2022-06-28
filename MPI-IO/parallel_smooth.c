/*
Assignment prepared by Sidharth Kumar (kumar.sidharth87@gmail.com)
*/

#include <unistd.h>
#include <stdarg.h>
#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <mpi.h>


static void parse_args(int argc, char **argv);

// Global dimension of dataset 25600 x 25600
static int  gbs[2] = {25600, 25600};

// Default of 1
static int  stencil_size = 1;

// Name of file to read
static char file_name[512] = "bonsai_collage_25600x25600_uint8.raw";

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);

  int rank, nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Offset offset;
  MPI_File file;
  MPI_Status status;

  parse_args(argc, argv);


  // READ THE FILE IN PARALLEL
  double io_read_start = MPI_Wtime();
  int totalDataSize = 655360000;
  int dividedSize = totalDataSize/nprocs;
  int *buffer;
  int offset = dividedSize*rank;
  buffer = (int*) mallac(dividedSize*sizeof(int));
  MPI_File_open(MPI_COMM_WORLD, file_name, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
  MPI_File_read_at_all(file, offset, buffer, dividedSize, MPI_INT, &status)
  MPI_File_close(file)
  double io_read_end = MPI_Wtime();
  


  // GHOST (HALO) CELL EXCHANGE
  double communication_start = MPI_Wtime();
  // Allocate buffer to recieve data from neighboring process (hint: will depend on the stencil point size), or use the local buffer of previous step to recieve halo data
  // Identify rank of neighboring process (hint: rank - 1 and rank + 1)
  // Make sure you consider the edge processes rank 0 and rank nprocs - 1, they will recieve and send data from only one process instead of two processes
  // Use MPI_Isend and MPI_Irecv for communication
  // You can or cannot use MPI datatypes for communication
  double communication_end = MPI_Wtime();




  // PERFORM THE ACTUAL BLURING OPERATION
  double compute_start = MPI_Wtime();
  // allocate a new buffer of size equal to local "buffer", that will store the value of the blured image
  // You can skip the border pixels for bluring. Start from (1,1) to (25600 - 1, 25600 - 1)
  // My pixel value = (my pixel value) / 9 + (sum of all neigbor pixel value) / 9
  // To perform the previous step you need to use the ghost cell you obtained in the previous step
  double compute_end = MPI_Wtime();



  // WRITE THE FILE IN PARALLEL (EXACT OPPOSITE of THE FIRST STEP)
  double io_write_start = MPI_Wtime();
  MPI_File_open(MPI_COMM_WORLD, "newFile.raw", MPI_MODE_CREATE|MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
  MPI_File_write_at_all(file, offset, buffer, dividedSize, MPI_INT, &status)
  MPI_File_close(file)
  double io_write_end = MPI_Wtime();


  double total_time = io_write_end - io_read_start;
  double max_time;
  MPI_Allreduce(&total_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
  if (max_time == total_time)
  {
    printf("Time take to blur %d x %d image is %f \n Time decomposistion (IO + COMM + COMP + IO): %f + %f + %f + %f \n", gbs[0], gbs[1], max_time, (io_read_end - io_read_start), 
    (communication_end - communication_start), 
    (compute_end - compute_start), 
    (io_write_end - io_write_start));
  }


  MPI_Finalize();
}


static void parse_args(int argc, char **argv)
{
  char flags[] = "g:s:f:";
  int one_opt = 0;
  int with_rst = 0;

  while ((one_opt = getopt(argc, argv, flags)) != EOF)
  {
    /* postpone error checking for after while loop */
    switch (one_opt)
    {
    case('g'): // global dimension
      if ((sscanf(optarg, "%dx%dx%d", &gbs[0], &gbs[1], &gbs[2]) == EOF))
        exit(-1);
      break;

    case('s'): // local dimension
      if ((sscanf(optarg, "%d", &stencil_size) == EOF))
        exit(-1);
      break;

    case('f'): // output file name
      if (sprintf(file_name, "%s", optarg) < 0)
        exit(-1);  
      break;

    }
  }
}
