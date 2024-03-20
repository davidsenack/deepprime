#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <assert.h>

// Prototype of functions from main.c to test
void generate_large_number(mpz_t num);
void trial_division(const mpz_t start, const mpz_t end);

// Test function for generate_large_number
void test_generate_large_number() {
    mpz_t num;
    mpz_init(num);
    generate_large_number(num);
    // Check if the number is not zero, assuming generate_large_number should generate a non-zero number
    assert(mpz_sgn(num) != 0);
    mpz_clear(num);
    printf("test_generate_large_number passed.\n");
}

// Test function for trial_division
void test_trial_division() {
    mpz_t start, end;
    mpz_init_set_ui(start, 1); // Start from 1
    mpz_init_set_ui(end, 100); // End at 100, small range for quick test
    trial_division(start, end);
    // This test is more about checking if the function runs without error, as it prints output to console
    printf("test_trial_division passed.\n");
    mpz_clear(start);
    mpz_clear(end);
}

int main() {
    test_generate_large_number();
    test_trial_division();
    return 0;
}
