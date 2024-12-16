#include <stdio.h>
#include <stdlib.h>

void xor_files(const char *file1, const char *file2, const char *output) {
    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "rb");
    FILE *out = fopen(output, "wb");
    if (!f1 || !f2 || !out) {
        perror("Error opening files");
        exit(EXIT_FAILURE);
    }

    int byte1, byte2;
    while (1) {
        byte1 = fgetc(f1);
        byte2 = fgetc(f2);

        if (byte1 == EOF && byte2 == EOF) break;

        if (byte1 == EOF) byte1 = 0; // Pad file1 with 0x00
        if (byte2 == EOF) byte2 = 0; // Pad file2 with 0x00

        fputc(byte1 ^ byte2, out);
    }

    fclose(f1);
    fclose(f2);
    fclose(out);
}

void decrypt_with_plaintext(const char *xor_file, const char *known_plain, const char *result) {
    FILE *xor_f = fopen(xor_file, "rb");
    FILE *plain_f = fopen(known_plain, "rb");
    FILE *result_f = fopen(result, "wb");
    if (!xor_f || !plain_f || !result_f) {
        perror("Error opening files");
        exit(EXIT_FAILURE);
    }

    int xor_byte, plain_byte;
    while ((xor_byte = fgetc(xor_f)) != EOF && (plain_byte = fgetc(plain_f)) != EOF) {
        fputc(xor_byte ^ plain_byte, result_f);
    }

    fclose(xor_f);
    fclose(plain_f);
    fclose(result_f);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <cipher1> <cipher2> <known_plain> <result>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *cipher1 = argv[1];
    const char *cipher2 = argv[2];
    const char *known_plain = argv[3];
    const char *result = argv[4];

    const char *temp_file = "temp_xor.bin";

    // Step 1: XOR the two ciphertexts to derive XOR of plaintexts
    xor_files(cipher1, cipher2, temp_file);

    // Step 2: Use known plaintext to decrypt the unknown plaintext
    decrypt_with_plaintext(temp_file, known_plain, result);

    // Clean up temporary file
    remove(temp_file);

    printf("Decryption completed. Output saved to %s\n", result);
    return EXIT_SUCCESS;
}
