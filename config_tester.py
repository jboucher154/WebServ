import os
import subprocess

def test_config_validator(bad_config_files, executable_path):
    # Check if the directory exists
    if not os.path.exists(bad_config_files):
        print(f"Error: Directory '{bad_config_files}' does not exist.")
        return

    # Check if the executable exists
    if not os.path.exists(executable_path):
        print(f"Error: Executable '{executable_path}' not found.")
        return

    # Iterate over files in the directory
    for filename in os.listdir(bad_config_files):
        file_path = os.path.join(bad_config_files, filename)

        # Check if the path is a file (not a directory)
        if os.path.isfile(file_path):
            # Call the executable with the file
            command = [executable_path, file_path]

            try:
                # Run the command
                subprocess.run(command, check=True)
                print(f"Executable ran successfully on {filename}")
            except subprocess.CalledProcessError as e:
                if e.returncode == 1:
                    print(f"Executable exited with status code 1 on {filename}")
                else:
                    print(f"Error running executable on {filename}. Status code: {e.returncode}")

# Example usage
bad_config_files = './config/bad/'
executable_path = './WebServ'

test_config_validator(bad_config_files, executable_path)
