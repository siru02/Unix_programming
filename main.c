#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <dirent.h>

#define MATRIX_SIZE 128
#define NUM_CLIENTS 8

void split_matrix_4(const char* fileName);
void split_matrix_8(const char* fileName);

int main()
{
    char filename[256];
    printf("enter 'input file name': ");
    scanf("%s", filename);
    split_matrix_4(filename);
    return 0;
}

// 4x4로 분할하는 함수
void split_matrix_4(const char* fileName)
{
    // 입력파일 열기
    FILE *fp = fopen(fileName, "r");
    if (fp == NULL){
        perror("fopen input");
        exit(1);
    }

    // matrix 읽어오기
    int matrix[MATRIX_SIZE][MATRIX_SIZE];
    for (int i = 0; i < MATRIX_SIZE; i++){
        for (int j = 0; j < MATRIX_SIZE; j++){
            if (fscanf(fp, "%d", &matrix[i][j]) != 1) {
                perror("fscanf");
                fclose(fp);
                exit(1);
            }
        }
    }
    fclose(fp);

    // 클라이언트 개수만큼 파일을 생성
    FILE *files[NUM_CLIENTS];
    for (int i = 0; i < NUM_CLIENTS; ++i) {
        char filename[256];
        snprintf(filename, sizeof(filename), "4x4_%d.txt", i);
        files[i] = fopen(filename, "w");
        if (files[i] == NULL) {
            perror("fopen");
            for (int j = 0; j < i; ++j) {
                fclose(files[j]);
            }
            exit(1);
        }
    }
    
    int block_size = MATRIX_SIZE / 4;
    // 4x4 블록으로 나누어 각 클라이언트(파일)에 저장
    for (int block_row = 0; block_row < 4; ++block_row) {
        for (int block_col = 0; block_col < 4; ++block_col) {
            int file_index = (block_row * 4 + block_col) % NUM_CLIENTS;
            
            for (int i = 0; i < block_size; ++i) {
                for (int j = 0; j < block_size; ++j) {
                    fprintf(files[file_index], "%d ", matrix[block_row * block_size + i][block_col * block_size + j]);
                }
                fprintf(files[file_index], "\n"); //확인을 위해서 줄바꿈 임시 추가
            }
            fprintf(files[file_index], "\n"); // 확인을 위해서 줄바꿈 임시 추가
        }
    }

    // 파일 포인터 닫기
    for (int i = 0; i < NUM_CLIENTS; i++) {
        fclose(files[i]);
    }
}

// 8x8로 분할하는 함수
void split_matrix_8(const char* fileName)
{
    // 입력파일 열기
    FILE *fp = fopen(fileName, "r");
    if (fp == NULL){
        perror("fopen input");
        exit(1);
    }

    // matrix 읽어오기
    int matrix[MATRIX_SIZE][MATRIX_SIZE];
    for (int i = 0; i < MATRIX_SIZE; i++){
        for (int j = 0; j < MATRIX_SIZE; j++){
            if (fscanf(fp, "%d", &matrix[i][j]) != 1) {
                perror("fscanf");
                fclose(fp);
                exit(1);
            }
        }
    }
    fclose(fp);

    // 클라이언트 개수만큼 파일을 생성
    FILE *files[NUM_CLIENTS];
    for (int i = 0; i < NUM_CLIENTS; ++i) {
        char filename[256];
        snprintf(filename, sizeof(filename), "block_%d.txt", i);
        files[i] = fopen(filename, "w");
        if (files[i] == NULL) {
            perror("fopen");
            for (int j = 0; j < i; ++j) {
                fclose(files[j]);
            }
            exit(1);
        }
    }
    
    int block_size = MATRIX_SIZE / 4;
    // 4x4 블록으로 나누어 각 클라이언트(파일)에 저장
    for (int block_row = 0; block_row < 4; ++block_row) {
        for (int block_col = 0; block_col < 4; ++block_col) {
            int file_index = (block_row + block_col) % NUM_CLIENTS;
            
            for (int i = 0; i < block_size; ++i) {
                for (int j = 0; j < block_size; ++j) {
                    fprintf(files[file_index], "%d ", matrix[block_row * block_size + i][block_col * block_size + j]);
                }
                fprintf(files[file_index], "\n"); //확인을 위해서 줄바꿈 임시 추가
            }
            fprintf(files[file_index], "\n"); // 확인을 위해서 줄바꿈 임시 추가
        }
    }

    // 파일 포인터 닫기
    for (int i = 0; i < NUM_CLIENTS; i++) {
        fclose(files[i]);
    }
}