#!/bin/bash
#SBATCH --job-name=prime_finder
#SBATCH --output=prime_finder_%j.out
#SBATCH --error=prime_finder_%j.err
#SBATCH --nodes=200
#SBATCH --ntasks-per-node=1
#SBATCH --time=24:00:00
#SBATCH --partition=main

module load gcc/9.2.0
module load openmpi/4.0.2
module load gmp/6.1.2

mpirun ./main
