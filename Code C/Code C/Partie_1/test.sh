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
    key=$5

    # Set the output and decrypted file paths based on input file
    output_file_cbc="$OUTPUT_DIR/encrypted_cbc_$(basename "$input")"
    output_file_mask="$OUTPUT_DIR/encrypted_mask_$(basename "$input")"
    output_file_xor="$OUTPUT_DIR/encrypted_xor_$(basename "$input")"

    decrypted_file_cbc="$DECRYPTED_DIR/decrypted_cbc_$(basename "$input")"
    decrypted_file_mask="$DECRYPTED_DIR/decrypted_mask_$(basename "$input")"
    decrypted_file_xor="$DECRYPTED_DIR/decrypted_xor_$(basename "$input")"
    
    iv_option="-v $IV_FILE"  # Use IV only for CBC

    echo "-----------------------------------"
    echo "Test: $description on file $input"
    
    # Construct the command to run based on method
    if [[ "$method" == "cbc-crypt" ]]; then
        cmd="./sym_crypt -i $input -o $output_file_cbc $key_option $key -m cbc-crypt $iv_option"
    elif [[ "$method" == "xor" ]]; then
        cmd="./sym_crypt -i $input -o $output_file_xor $key_option $key -m xor"
    else
        cmd="./sym_crypt -i $input -o $output_file_mask $key_option $key -m mask"
    fi
    echo "Command: $cmd"
    
    # Run the encryption command
    eval $cmd

   # Check if any output files were generated
    if [[ -f "$output_file_cbc" || -f "$output_file_mask" || -f "$output_file_xor" ]]; then
        echo "Encryption success"
    else
        echo "Test failed: No output generated"
    fi

    # For CBC method, run decryption and compare with the original
    if [[ "$method" == "cbc-crypt" ]]; then
        # Run decryption test on CBC
        decryption_cmd="./sym_crypt -i $output_file_cbc -o $decrypted_file_cbc $key_option $key -m cbc-uncrypt $iv_option"
        echo "Decryption Command: $decryption_cmd"
        eval $decryption_cmd

        # Verify if the decrypted content matches the original
        if cmp -s "$input" "$decrypted_file_cbc"; then
            echo "Decryption CBC successful: Decrypted content matches original."
        else
            echo "Decryption CBC failed: Decrypted content does not match original."
        fi
    elif [[ "$method" == "xor" ]]; then
        # Run decryption test on XOR
        decryption_cmd="./sym_crypt -i $output_file_xor -o $decrypted_file_xor $key_option $key -m xor"
        echo "Decryption Command: $decryption_cmd"
        eval $decryption_cmd

        # Verify if the decrypted content matches the original
        if cmp -s "$input" "$decrypted_file_xor"; then
            echo "Decryption XOR successful: Decrypted content matches original."
        else
            echo "Decryption XOR failed: Decrypted content does not match original."
        fi
    else
        # Run decryption test on Mask
        decryption_cmd="./sym_crypt -i $output_file_mask -o $decrypted_file_mask $key_option $key -m mask"
        echo "Decryption Command: $decryption_cmd"
        eval $decryption_cmd

        # Verify if the decrypted content matches the original
        if cmp -s "$input" "$decrypted_file_mask"; then
            echo "Decryption Mask successful: Decrypted content matches original."
        else
            echo "Decryption Mask failed: Decrypted content does not match original."
        fi
    fi
    echo "-----------------------------------"
}

# Loop through each test file in the directory
for input_file in "$INPUT_DIR"/*; do
    # Skip if not a regular file
    [ -f "$input_file" ] || continue

    # Run CBC encryption and decryption test with key file
    run_test "CBC Encryption/Decryption" "$input_file" "cbc-crypt" "-f" "$KEY_FILE"

    #Run CBC encryption and decryption test with key 

    run_test "CBC Encryption/Decryption" "$input_file" "cbc-crypt" "-k" "encryption_key"

    # Run XOR encryption test with an inline key
    run_test "XOR Encryption " "$input_file" "xor" "-k" "encryption_key"

    # Run XOR encryption test with a key file
    run_test "XOR Encryption " "$input_file" "xor" "-f" "$KEY_FILE"

    # Run Mask encryption test with generated key length of 8
    run_test "Mask Encryption " "$input_file" "mask" "-k" "encryption_key"

    # Run Mask encryption test with a key file
    run_test "Mask Encryption " "$input_file" "mask" "-f" "$KEY_FILE"
done

# Clean up
make clean
echo "All tests completed."
