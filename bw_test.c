#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv) {
  int sz, myid;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &sz);

  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  int *buf = malloc(4*1024*1024);
  int partner = (myid + sz/2) % sz;
  int N = 10000;

  double bw_in_MBs;

  unsigned int packet_size;
  int i;
  for (packet_size = 128; packet_size <= 4*1024*1024; packet_size *= 2) {
    if (myid < partner) {
      double t1 = MPI_Wtime();

      for (i = 0; i < N; ++i) {
        MPI_Recv(buf, packet_size / sizeof(int), MPI_INT, partner, 0,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }

      double t2 = MPI_Wtime();

      bw_in_MBs = packet_size * N / (1024*1024) / (t2-t1);
    } else {
      for (i = 0; i < N; ++i) {
        MPI_Send(buf, packet_size / sizeof(int), MPI_INT, partner, 0, MPI_COMM_WORLD);
      }
    }

    if (myid == 0) {
      for (i = 1; i < sz/2; ++i) {
        double recv_bw;
        MPI_Recv(&recv_bw, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        bw_in_MBs += recv_bw;
      }
      printf("%d: %f\n", packet_size, bw_in_MBs / (sz/2));
    } else if (myid < sz/2) {
      MPI_Send(&bw_in_MBs, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
  }

  free(buf);

  MPI_Finalize();
  exit(0);
}

