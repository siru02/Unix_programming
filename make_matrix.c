#include <stdio.h>
#include <stdlib.h>

#define MATRIX_SIZE 128

void generate_matrix(const char *output_file) {
    FILE *out = fopen(output_file, "w");
    if (out == NULL) {
        perror("Failed to create output file");
        exit(EXIT_FAILURE);
    }

    // 128x128 매트릭스 생성
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            fprintf(out, "%d ", i * MATRIX_SIZE + j); // 값: i * 128 + j
        }
        fprintf(out, "\n"); // 행 구분
    }

    fclose(out);
    printf("Matrix generated and saved to %s\n", output_file);
}

int main() {
    const char *output_file = "matrix.txt"; // 출력 파일 이름
    generate_matrix(output_file);          // 매트릭스 생성
    return 0;
}
