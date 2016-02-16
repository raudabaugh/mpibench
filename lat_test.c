#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv) {
  int sz, myid;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &sz);

  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  int buf;
  int partner = (myid + sz/2) % sz;
  int N = 100000;

  double latency_in_us;

  if (myid < partner) {
    double t1 = MPI_Wtime();

    for (int i = 0; i < N; ++i) {
      MPI_Send(&buf, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
      MPI_Recv(&buf, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    double t2 = MPI_Wtime();

    latency_in_us = 1000000*(t2 - t1)/N/2.0;
  } else {
    for (int i = 0; i < N; ++i) {
      MPI_Recv(&buf, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Send(&buf, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
    }
  }

  if (myid == 0) {
    for (int i = 1; i < sz/2; ++i) {
      double recv_latency;
      MPI_Recv(&recv_latency, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      latency_in_us += recv_latency;
    }
    printf("%f\n", latency_in_us / (sz/2));
  } else if (myid < sz/2) {
    MPI_Send(&latency_in_us, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  exit(0);
}

