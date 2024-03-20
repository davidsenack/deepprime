
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>
#include <gmp.h>
#include <time.h>

#define MASTER_RANK 0

void generate_large_number(mpz_t num) {
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));
    mpz_urandomb(num, state, 100000000 * 3.32192809489);
    gmp_randclear(state);
}

void trial_division(const mpz_t start, const mpz_t end) {
    mpz_t n, divisor;
    mpz_inits(n, divisor, NULL);
    mpz_set(n, start);
    while (mpz_cmp(n, end) <= 0) {
        int is_prime = 1;
        mpz_set_ui(divisor, 2);
        while (mpz_cmp(divisor, n) < 0) {
            if (mpz_divisible_p(n, divisor)) {
                is_prime = 0;
                break;
            }
            mpz_add_ui(divisor, divisor, 1);
        }
        if (is_prime) {
            gmp_printf("Prime candidate: %Zd\n", n);
        }
        mpz_add_ui(n, n, 1);
    }
    mpz_clears(n, divisor, NULL);
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    mpz_t start, end;
    mpz_inits(start, end, NULL);

    if (rank == MASTER_RANK) {
        generate_large_number(start);
        mpz_add_ui(end, start, 1000000);
    }

    size_t start_size = 0, end_size = 0;
    void *start_export = mpz_export(NULL, &start_size, 0, sizeof(mp_limb_t), 0, 0, start);
    void *end_export = mpz_export(NULL, &end_size, 0, sizeof(mp_limb_t), 0, 0, end);
    MPI_Bcast(start_export, start_size, MPI_BYTE, MASTER_RANK, MPI_COMM_WORLD);
    MPI_Bcast(end_export, end_size, MPI_BYTE, MASTER_RANK, MPI_COMM_WORLD);

    mpz_t range, local_start, local_end;
    mpz_inits(range, local_start, local_end, NULL);
    mpz_sub(range, end, start);
    mpz_cdiv_q_ui(range, range, size);
    mpz_mul_ui(local_start, range, rank);
    mpz_add(local_start, local_start, start);
    if (rank == size - 1) {
        mpz_set(local_end, end);
    } else {
        mpz_add(local_end, local_start, range);
        mpz_sub_ui(local_end, local_end, 1);
    }

    #pragma omp parallel
    {
        mpz_t omp_start, omp_end;
        mpz_inits(omp_start, omp_end, NULL);
        unsigned long thread_id = omp_get_thread_num();
        unsigned long num_threads = omp_get_num_threads();
        mpz_cdiv_q_ui(omp_start, range, num_threads);
        mpz_mul_ui(omp_start, omp_start, thread_id);
        mpz_add(omp_start, omp_start, local_start);
        if (thread_id == num_threads - 1) {
            mpz_set(omp_end, local_end);
        } else {
            mpz_add(omp_end, omp_start, omp_start);
            mpz_sub_ui(omp_end, omp_end, 1);
        }
        trial_division(omp_start, omp_end);
        mpz_clears(omp_start, omp_end, NULL);
    }

    mpz_clears(start, end, range, local_start, local_end, NULL);
    free(start_export);
    free(end_export);

    MPI_Finalize();
    return 0;
}

