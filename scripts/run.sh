#!/bin/bash

# Clone the GitHub repository
git clone https://github.com/your-username/your-repository.git
cd your-repository

# Compile the C program
gcc -o your_program your_program.c -lgmp

# Prepare SLURM job script
cat <<EOF > slurm_job.sh
#!/bin/bash

#SBATCH --nodes=2             # Number of nodes
#SBATCH --ntasks-per-node=1  # Number of tasks (processes) per node
#SBATCH --time=00:30:00      # Maximum runtime of the job (HH:MM:SS)
#SBATCH --job-name=my_program # Name of the job

# Load any necessary modules or set up environment variables
# (e.g., module load gcc)

# Run your program
./your_program    # Update with the name of your compiled program
EOF

# Submit the SLURM job
sbatch slurm_job.sh

# Monitor job status
echo "Job submitted. Monitoring job status..."
squeue -u $USER  # Replace $USER with your username

