#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char **argv) {
  int sz, myid;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &sz);

  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  int buf;
  int partner = (myid + sz/2) % sz;
  int N = 100000;
  int i;
  double v[N];
  double u[N];

  // Initialize v and u
  srand((unsigned)time(NULL));
  for (i = 0; i < N; ++i) {
    v[i] = ((double)rand()/(double)RAND_MAX);
    u[i] = ((double)rand()/(double)RAND_MAX);
  }

  double us_per_op;

  double t1 = MPI_Wtime();

  double dot_product = 0.0;
  for (i = 0; i < N; ++i) {
    dot_product += v[i]*u[i];
  }

  double t2 = MPI_Wtime();

  us_per_op = 1000000*(t2 - t1)/N/2;

  if (myid == 0) {
    for (i = 1; i < sz; ++i) {
      double recv_time;
      MPI_Recv(&recv_time, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      us_per_op += recv_time;
    }
    printf("%f\n", us_per_op / sz);
  } else {
    MPI_Send(&us_per_op, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  exit(0);
}

