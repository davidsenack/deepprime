# Update and install necessary packages
sudo apt-get update
sudo apt-get install -y wget gcc libgmp-dev libopenmpi-dev make

# Define the GitHub URL for the file to be downloaded
GITHUB_FILE_URL="https://github.com/user/repo/path/to/file"

# Define the paths for the SLURM job files
TEST_JOB_FILE="scripts/main_job.slurm"
MAIN_JOB_FILE="scripts/main_job.slurm"

# Download the file from GitHub
wget $GITHUB_FILE_URL -O downloaded_file

# Check if the download was successful
if [ $? -eq 0 ]; then
    echo "File downloaded successfully."
    
    # Compile the project
    cd src && make all && cd ..
    
    # Run the test job
    sbatch $TEST_JOB_FILE
    TEST_JOB_EXIT_CODE=$?
    
    # Check if the test job executed successfully
    if [ $TEST_JOB_EXIT_CODE -eq 0 ]; then
        echo "Test job executed successfully."
        
        # Run the main job
        sbatch $MAIN_JOB_FILE
        MAIN_JOB_EXIT_CODE=$?
        
        # Check if the main job executed successfully
        if [ $MAIN_JOB_EXIT_CODE -eq 0 ]; then
            echo "Main job executed successfully."
        else
            echo "Failed to execute the main job."
        fi
    else
        echo "Failed to execute the test job."
    fi
else
    echo "Failed to download the file."
fi
