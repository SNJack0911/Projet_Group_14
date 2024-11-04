#!/bin/bash

# Compile the program using the makefile
make || { echo "Compilation failed"; exit 1; }

# Directory setup for testing
INPUT_DIR="test_file"           # Directory containing test input files
OUTPUT_DIR="encrypted_files"     # Directory for storing output files
DECRYPTED_DIR="decrypted_files"  # Directory for storing decrypted files
KEY_FILE="key.txt"
IV_FILE="iv.txt"
LOG_FILE="log.txt"

# Create output directories and key files for testing
mkdir -p "$OUTPUT_DIR" "$DECRYPTED_DIR"
echo "ratata" > "$KEY_FILE"
echo "initializationvector" > "$IV_FILE"

# Function to run a test case
run_test() {
    description=$1
    input=$2
    method=$3
    key_option=$4

    # Set the output and decrypted file paths based on input file
    output_file="$OUTPUT_DIR/encrypted_$(basename "$input")"
    decrypted_file="$DECRYPTED_DIR/decrypted_$(basename "$input")"
    iv_option="-v $IV_FILE"  # Use IV only for CBC

    echo "-----------------------------------"
    echo "Test: $description on file $input"
    
    # Construct the command to run based on method
    if [[ "$method" == "cbc-crypt" ]]; then
        cmd="./sym_crypt -i $input -o $output_file -k $key_option -m cbc-crypt $iv_option"
    elif [[ "$method" == "xor" ]]; then
        cmd="./sym_crypt -i $input -o $output_file -k $key_option -m xor"
    else
        cmd="./sym_crypt -i $input -o $output_file -k $key_option -m mask"
    fi
    echo "Command: $cmd"
    
    # Run the encryption command
    eval $cmd

    # Display the output or an error if the output file was not generated
    if [ -f "$output_file" ]; then
        echo "Output file content (hexadecimal):"
        xxd "$output_file"
        echo
    else
        echo "Test failed: No output generated"
    fi

    # For CBC methods, run decryption and compare with the original
    if [[ "$method" == "cbc-crypt" ]]; then
        # Run decryption test
        decryption_cmd="./sym_crypt -i $output_file -o $decrypted_file -k $key_option -m cbc-uncrypt $iv_option"
        echo "Decryption Command: $decryption_cmd"
        eval $decryption_cmd

        # Verify if the decrypted content matches the original
        if cmp -s "$input" "$decrypted_file"; then
            echo "Decryption successful: Decrypted content matches original."
        else
            echo "Decryption failed: Decrypted content does not match original."
        fi
    fi
    echo "-----------------------------------"
}

# Loop through each test file in the directory
for input_file in "$INPUT_DIR"/*; do
    # Skip if not a regular file
    [ -f "$input_file" ] || continue

    # Run CBC encryption and decryption test
    run_test "CBC Encryption/Decryption" "$input_file" "cbc-crypt" "$KEY_FILE"

    # Run XOR encryption test with an inline key
    run_test "XOR Encryption with inline key" "$input_file" "xor" "encryptionkey"

    # Run XOR encryption test with a key file
    run_test "XOR Encryption with key file" "$input_file" "xor" "$KEY_FILE"

    # Run Mask encryption test with generated key length of 8
    run_test "Mask Encryption with generated key" "$input_file" "mask" "ratatatata"

    # Run Mask encryption test with a key file
    run_test "Mask Encryption with key file" "$input_file" "mask" "$KEY_FILE"
done

# Clean up
make clean
echo "All tests completed."
