#include <iostream>
#include <mpi.h>
#include <cmath>

using namespace std;
int N = 100; // Number of particle
int constMass = 1000000; // For massed of particle
int deltaTime = 0.1; // Time increment
int G = 6.67300e-11 ; // Gravitational constant
int sharedParticle ; // variable to store particle divided based on the ranks
int pindex ; // variable to point to certain particle during calculation
int my_rank, numProc;
int userTime = 10; // Number of iteration

// Here the particle are assumed to be in 2D.
// position of particle 
typedef struct {
    double px, py;
} Position;

// velocity of particle
typedef struct {
    double vx, vy;
} Velocity;

// force experienced by particle 
typedef struct {
    double fx, fy;
} Force;

Position* position;   // Current positions for all particles
Velocity* ivelocity;  // Initial velocity for all particles
Velocity* velocity;   // Velocity of particles in current processor
Force* force;         // Force experienced by all particles
double *mass;	      // Mass of particles

// MPI data type to support passing particles.
MPI_Datatype MPI_POSITION;
MPI_Datatype MPI_VELOCITY;

// random number generator < 1000
double generate_random(){
    double num = rand() % 1000;
    return num;
}

// set initial velocity to zero
void init_velocity(int sharedParticle){
   int i;
   for (i = 0; i < sharedParticle; i++){
      velocity[i].vx = 0;
      velocity[i].vy = 0;
   }
}

// set random values to required fields
void setRequireData(){
    srand(time(0)); 
    for (int i = 0; i < N; i++) {
        mass[i] = constMass * generate_random();
        position[i].px = generate_random();
        position[i].py = generate_random();
        ivelocity[i].vx = generate_random();
        ivelocity[i].vy = generate_random();
    }
}

double compute_distance(Position a, Position b){
    return sqrt(pow((a.px - b.px), 2.0) +
               pow((a.py - b.py), 2.0));
}

void compute_force(){
   int i, j;   
   for (i = 0; i < sharedParticle; i++) {
      force[i].fx = 0.0;
      force[i].fy = 0.0;
      for (j = 0; j < N; j++){
         if (j == (i + pindex)) continue; 
        double d = compute_distance(position[i + pindex], position[j]);
         // Compute grativational force according to Newtonian's law
         double f = (G * (mass[i + pindex] * mass[j]) / (pow(d, 2.0)));
         // Resolve forces in each direction
         force[i].fx += f * ((position[j].px - position[i + pindex].px) / d);
         force[i].fy += f * ((position[j].py - position[i + pindex].py) / d);
      }
   }
}

void compute_velocity(){
   int i;
   for (i = 0; i < sharedParticle; i++) {
      velocity[i].vx += (force[i].fx / mass[i + pindex]) * deltaTime;
      velocity[i].vy += (force[i].fy / mass[i + pindex]) * deltaTime;
   }
}

void compute_positions(){
   int i;
   for (i = 0; i < sharedParticle; i++) {
      position[i + pindex].px += velocity[i].vx * deltaTime;
      position[i + pindex].py += velocity[i].vy * deltaTime;
   }
}

void runNbodySolver(){
    // Broadcast mass and position to all members in the group and scatter initial velocity
    MPI_Bcast(mass, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(position, N, MPI_POSITION, 0, MPI_COMM_WORLD);
    MPI_Scatter(ivelocity, sharedParticle, MPI_VELOCITY, velocity, sharedParticle, MPI_VELOCITY, 0, MPI_COMM_WORLD);

    for (int i = 0; i < userTime; i++) {
        compute_force();
        compute_velocity();
        compute_positions();
        MPI_Allgather(position + (my_rank * sharedParticle), sharedParticle, MPI_POSITION, position, sharedParticle, MPI_POSITION, MPI_COMM_WORLD); 
    }

    if (my_rank == 0){
        for (int i = 0; i < N; i++) {
            cout << "Position at for Particle Number: " << i << " at X direction is: " << position[i].px << " and Y direction is: " << position[i].py << "\n";
        }
    }
}

int main(int argc, char* argv[]) {

    // Initialize MPI
	int mpiErr = MPI_Init(&argc, &argv);
	mpiErr = MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	mpiErr = MPI_Comm_size(MPI_COMM_WORLD, &numProc);
    // Initialzie new data type to support two values.
    MPI_Type_contiguous(2, MPI_DOUBLE, &MPI_POSITION);
    MPI_Type_contiguous(2, MPI_DOUBLE, &MPI_VELOCITY);
    MPI_Type_commit(&MPI_POSITION);
    MPI_Type_commit(&MPI_VELOCITY);
    // variable used to share particle in ranks
    sharedParticle = N / numProc;
    pindex = my_rank * sharedParticle;
    // Initialize memory
    mass = (double *) malloc(N * sizeof(double));
    position = (Position *) malloc(N * sizeof(Position));
    ivelocity = (Velocity *) malloc(N * sizeof(Velocity));
    velocity = (Velocity *) malloc(sharedParticle * sizeof(Velocity));
    force = (Force *) malloc(sharedParticle * sizeof(Force));

    init_velocity(sharedParticle);

    if (my_rank == 0) {
        setRequireData();
    }

    runNbodySolver();

    mpiErr = MPI_Finalize();
}