#include <stdio.h>
#include <stdlib.h>

#define MATRIX_SIZE 128

void generate_matrix_binary(const char *output_file) {
    FILE *out = fopen(output_file, "wb"); // 바이너리 모드로 파일 열기
    if (out == NULL) {
        perror("Failed to create output file");
        exit(EXIT_FAILURE);
    }

    // 128x128 매트릭스 생성 및 바이너리 파일로 저장
    for (int i = 0; i < MATRIX_SIZE; i++) {
        int row[MATRIX_SIZE]; // 행 데이터를 임시 저장할 배열
        for (int j = 0; j < MATRIX_SIZE; j++) {
            row[j] = i * MATRIX_SIZE + j; // 값: i * 128 + j
        }
        fwrite(row, sizeof(int), MATRIX_SIZE, out); // 한 행씩 바이너리로 저장
    }

    fclose(out);
    printf("Matrix generated and saved to %s (binary)\n", output_file);
}

int main() {
    const char *output_file = "matrix.bin"; // 출력 파일 이름
    generate_matrix_binary(output_file);    // 바이너리 매트릭스 생성
    return 0;
}
