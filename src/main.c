#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <gmp.h>

#define MASTER_RANK 0
#define NUM_THREADS 2

void trial_division(const mpz_t start, const mpz_t end) {
    // Loop through integers in the range [start, end]
    mpz_t n, divisor;
    mpz_inits(n, divisor, NULL);

    mpz_set(n, start);
    while (mpz_cmp(n, end) <= 0) {
        int is_prime = 1;
        mpz_set_ui(divisor, 2);

        // Trial division algorithm
        while (mpz_cmp(divisor, n) < 0) {
            if (mpz_divisible_p(n, divisor)) {
                is_prime = 0;
                break;
            }
            mpz_add_ui(divisor, divisor, 1);
        }

        // If n is prime, print it
        if (is_prime) {
            printf("Prime candidate: %s\n", mpz_get_str(NULL, 10, n));
        }

        mpz_add_ui(n, n, 1); // Increment n
    }

    // Clear memory
    mpz_clears(n, divisor, NULL);
}


int main(int argc, char *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Initialize GMP
    mpz_t start, end;
    mpz_inits(start, end, NULL);

    // Set the range of integers to check
    if (rank == MASTER_RANK) {
        // Set range on master node
        mpz_set_str(start, "10000000000000000000000000000000000000000000000000000000000000000", 10);
        mpz_set_str(end, "10000000000000000000000000000000000000000000000000000000000000100", 10);
    }

    // Broadcast the range to all nodes
    MPI_Bcast(start, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);
    MPI_Bcast(end, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);

    // Perform trial division on the assigned range
    trial_division(start, end);

    // Gather results from all nodes
    // Note: In this example, each node has the same result as the master node,
    // so we can simply print the result on each node. In a real scenario,
    // you would need to gather results from all nodes to the master node.
    printf("Rank %d: Prime candidates found in the range [%s, %s]\n", rank, mpz_get_str(NULL, 10, start), mpz_get_str(NULL, 10, end));

    // Finalize MPI
    MPI_Finalize();

    // Clear memory
    mpz_clears(start, end, NULL);

    return 0;
}

