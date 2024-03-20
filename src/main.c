
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h> // Include for GNU Multiple Precision Arithmetic Library
#include <time.h> // Include for time functions
#include <omp.h> // Include for OpenMP parallel programming support
#include <mpi.h> // Include for MPI library for parallel programming across multiple nodes

#define MASTER_RANK 0 // Define master process rank for MPI

// Function to generate a large random number using GMP library
void generate_large_number(mpz_t num) {
    gmp_randstate_t state; // Declare state for random number generation
    gmp_randinit_default(state); // Initialize state with default algorithm
    gmp_randseed_ui(state, time(NULL)); // Seed the state with current time
    mpz_urandomb(num, state, 100000000 * 3.32192809489); // Generate a large random number
    gmp_randclear(state); // Clear the state of the random number generator
}

// Function to perform trial division to find prime numbers in a given range, skipping even numbers
void trial_division(const mpz_t start, const mpz_t end) {
    mpz_t n, divisor; // Declare variables for the current number and divisor
    mpz_inits(n, divisor, NULL); // Initialize the variables
    // Ensure n starts from an odd number if start is even
    if (mpz_even_p(start)) mpz_add_ui(n, start, 1);
    else mpz_set(n, start);
    while (mpz_cmp(n, end) <= 0) { // Loop until n exceeds end
        int is_prime = 1; // Assume n is prime
        mpz_set_ui(divisor, 3); // Start divisor from 3, skipping 2 (even number)
        while (mpz_cmp(divisor, n) < 0) { // Loop until divisor equals n
            if (mpz_divisible_p(n, divisor)) { // Check if n is divisible by divisor
                is_prime = 0; // n is not prime
                break; // Exit loop
            }
            mpz_add_ui(divisor, divisor, 2); // Increment divisor by 2, skipping even numbers
        }
        if (is_prime) { // If n is prime
            gmp_printf("Prime candidate: %Zd\n", n); // Print the prime number
        }
        mpz_add_ui(n, n, 2); // Increment n by 2, skipping even numbers
    }
    mpz_clears(n, divisor, NULL); // Clear the variables
}

int main(int argc, char *argv[]) {
    int rank, size; // Declare variables for process rank and size
    MPI_Init(&argc, &argv); // Initialize MPI environment
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get the rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get the number of processes

    mpz_t start, end; // Declare variables for start and end of the range
    mpz_inits(start, end, NULL); // Initialize the variables

    if (rank == MASTER_RANK) { // If this is the master process
        generate_large_number(start); // Generate a large number as start
        mpz_add_ui(end, start, 1000000); // Set end to start + 1,000,000
    }

    // Prepare to broadcast the start and end of the range to all processes
    size_t start_size = 0, end_size = 0;
    void *start_export = mpz_export(NULL, &start_size, 0, sizeof(mp_limb_t), 0, 0, start);
    void *end_export = mpz_export(NULL, &end_size, 0, sizeof(mp_limb_t), 0, 0, end);
    MPI_Bcast(start_export, start_size, MPI_BYTE, MASTER_RANK, MPI_COMM_WORLD);
    MPI_Bcast(end_export, end_size, MPI_BYTE, MASTER_RANK, MPI_COMM_WORLD);

    mpz_t range, local_start, local_end; // Declare variables for local range calculation
    mpz_inits(range, local_start, local_end, NULL); // Initialize the variables
    mpz_sub(range, end, start); // Calculate the total range
    mpz_cdiv_q_ui(range, range, size); // Divide the range by the number of processes
    mpz_mul_ui(local_start, range, rank); // Calculate the start of the local range
    mpz_add(local_start, local_start, start); // Adjust the local start based on the global start
    if (rank == size - 1) { // If this is the last process
        mpz_set(local_end, end); // Set the local end to the global end
    } else {
        mpz_add(local_end, local_start, range); // Calculate the local end
        mpz_sub_ui(local_end, local_end, 1); // Adjust the local end
    }

    #pragma omp parallel // Start a parallel region with OpenMP
    {
        mpz_t omp_start, omp_end; // Declare variables for thread-local range calculation
        mpz_inits(omp_start, omp_end, NULL); // Initialize the variables
        unsigned long thread_id = omp_get_thread_num(); // Get the thread ID
        unsigned long num_threads = omp_get_num_threads(); // Get the number of threads
        mpz_cdiv_q_ui(omp_start, range, num_threads); // Divide the range by the number of threads
        mpz_mul_ui(omp_start, omp_start, thread_id); // Calculate the start of the thread-local range
        mpz_add(omp_start, omp_start, local_start); // Adjust the thread-local start based on the local start
        if (thread_id == num_threads - 1) { // If this is the last thread
            mpz_set(omp_end, local_end); // Set the thread-local end to the local end
        } else {
            mpz_add(omp_end, omp_start, range); // Correct calculation of omp_end
            mpz_sub_ui(omp_end, omp_end, 1); // Adjust the thread-local end
        }
        trial_division(omp_start, omp_end); // Perform trial division in the thread-local range
        mpz_clears(omp_start, omp_end, NULL); // Clear the variables
    }

    mpz_clears(start, end, range, local_start, local_end, NULL); // Clear the variables
    free(start_export); // Free the memory allocated for start_export
    free(end_export); // Free the memory allocated for end_export

    MPI_Finalize(); // Finalize the MPI environment
    return 0; // Return success
}
