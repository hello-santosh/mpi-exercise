This n-body problem is created based on the information from the book "An Introduction to Parallel Programming" Chapter 6
Based on information given the following pseudo code was derived and implemented into code. 

Get/SET input data;
if (my rank == 0) {
    for each particle
        Read masses[particle], pos[particle], vel[particle];
}
distribute the particles equally in each process 
calculate required particle properties
gather them all to print them
MPI Bcast(masses);
MPI Bcast(pos);
MPI Scatter(vel)
for each timestep {
    compute_force();
    compute_velocity();
    compute_positions();
    MPI_Allgather(pos);
}
Print positions of particles;
