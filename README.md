# Prime Number Finder

This project utilizes the GNU Multiple Precision Arithmetic Library (GMP), OpenMP, and MPI to find prime numbers in a distributed and parallel computing environment. It is designed to efficiently perform trial division to identify prime candidates within a very large range of numbers.

## Features

- **Large Number Generation**: Utilizes GMP to generate large random numbers as starting points.
- **Parallel Processing**: Leverages OpenMP for multi-threading on a single node and MPI for distributed computing across multiple nodes.
- **Efficient Prime Checking**: Implements trial division to check for prime numbers within a specified range.

## Requirements

- GNU Compiler Collection (GCC)
- GNU Multiple Precision Arithmetic Library (GMP)
- OpenMP for parallel programming
- MPI for distributed computing

## Building the Project

Navigate to the `src` directory and run the `make all` command to compile the project. This will generate the `main` executable for finding prime numbers and `test_main` for running tests.
