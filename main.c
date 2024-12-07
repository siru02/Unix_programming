#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <dirent.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MATRIX_SIZE 128
#define NUM_CLIENTS 8
#define SOCKET_PATH_PREFIX "./socket_"
#define BUFFER_SIZE 1024

void split_matrix_4(const char* fileName);
void split_matrix_8(const char* fileName);

int main()
{
    char filename[256];
    printf("enter 'input file name': ");
    scanf("%s", filename);
    split_matrix_4(filename);
    split_matrix_8(filename);


    // fork떠서 socket생성하고, client와의 연결을 기다리는
    pid_t pid = fork();
    if (pid < 0){
        perror("fork");
        exit(1);
    }
    
    int file_fd;
    if (pid == 0) { // 자식 프로세스 (server1 역할)
        // 자식 프로세스가 관리할 클라이언트들: 2, 3, 6, 7
        int clients_to_handle[] = {2, 3, 6, 7};
        file_fd = server_file(0);
        server_process(clients_to_handle, 4, "server1", 1);
    } else { // 부모 프로세스 (server0 역할)
        // 부모 프로세스가 관리할 클라이언트들: 0, 1, 4, 5
        int clients_to_handle[] = {0, 1, 4, 5};
        file_fd = server_file(1);
        server_process(clients_to_handle, 4, "server0", 0);

        // 모든 자식 프로세스가 종료될 때까지 대기
        wait(NULL);
    }

    return 0;
}

// UNIX소켓 생성하여 클라이언트와 연결
void server_process(int *clients, int num_clients, const char *server_name, int flag) {
    int serverFile_fd = server_file(flag); //서버파일 생성

    // 클라이언트별 루프로 파일에 기록
    for (int i = 0; i < num_clients; ++i) {
        int client_id = clients[i]; // {0 1 4 5}, {2 3 6 7}

        //./socket_serverN의 꼴로 소켓파일을 사용
        char socket_path[256];
        strcpy(socket_path, SOCKET_PATH_PREFIX); // ./socket_server
        int len = strlen(socket_path);
        socket_path[len] = '0' + flag; //./socket_server0 or ./socker_server1
        socket_path[len + 1] = '\0';

        // 소켓 생성 (저수준 소켓 파일 디스크립터 사용), UNIX TCP
        int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (server_fd < 0) {
            perror("socket");
            close(serverFile_fd);
            exit(1);
        }

        // 소켓 주소 설정
        struct sockaddr_un server_addr;
        memset(&server_addr, 0, sizeof(struct sockaddr_un)); // 소켓 데이터 비우기
        server_addr.sun_family = AF_UNIX; //뭐였더라
        strncpy(server_addr.sun_path, socket_path, sizeof(server_addr.sun_path) - 1);

        // 기존 소켓 파일 삭제 (이미 존재할 경우)
        unlink(socket_path);

        // 소켓 바인드 -> 소켓파일과 바인드
        if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_un)) == -1) {
            perror("bind");
            close(server_fd);
            close(serverFile_fd);
            exit(1);
        }

        // 소켓 리슨
        if (listen(server_fd, 5) == -1) {
            perror("listen");
            close(server_fd);
            close(serverFile_fd);
            exit(1);
        }

        printf("%s waiting for client %d on socket: %s\n", server_name, client_id, socket_path);

          // 클라이언트 연결 수락
        int client_fd = accept(server_fd, NULL, NULL); //클라이언트와 연결된 소켓버퍼의 fd값
        if (client_fd == -1) {
            perror("accept");
            close(server_fd);
            close(serverFile_fd);
            exit(1);
        }

        printf("%s connected to client %d\n", server_name, client_id);

        // 클라이언트와의 지속적인 통신 (저수준 read(), write() 사용)
        char buffer[BUFFER_SIZE]; //1024 byte
        ssize_t bytes_read;

        while (1) {
            // 클라이언트로부터 데이터 읽기
            ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1); // 소켓에서 값을 읽어온다
            if (bytes_read > 0) { //읽어온 내용이 있다면
                // server파일에 쓰기
                if (write(serverFile_fd, buffer, bytes_read) == -1) {
                    perror("write to file");
                    close(serverFile_fd);
                    close(server_fd);
                    close(client_fd);
                }

                // 클라이언트에게 응답 보내기
                const char *response = "Message received by server\n";
                if (write(client_fd, response, strlen(response)) == -1) {
                    perror("write");
                    close(serverFile_fd);
                    close(server_fd);
                    close(client_fd);
                }
            } 
            else if (bytes_read == 0) { //클라이언트가 소켓 연결을 닫으면 EOF를 전송하므로
                // 클라이언트가 연결을 종료함
                printf("Client %d disconnected from %s.\n", client_id, server_name);
                break;
            } 
            else {
                // 읽기 중 오류 발생
                perror("read");
                close(serverFile_fd);
                close(server_fd);
                close(client_fd);
                exit(1);
            }
        }

        // 클라이언트 소켓 파일 디스크립터 닫기
        close(client_fd);
        // 서버 소켓 파일 디스크립터 닫기
        close(server_fd);
    }
    close(serverFile_fd);
}

int server_file(int num) 
{
    char filename[128];
    if (num == 0) //부모프로세스
        strcpy(filename, "server0.bin");
    else //자식 프로세스
        strcpy(filename, "server1.bin");

    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        exit(1);
    }
    return fd;
}


//////////////////////////////////////////////////////////////////////////////////////////////

// 4x4로 분할하는 함수
void split_matrix_4(const char* fileName)
{
// 입력 파일 열기 (바이너리 읽기 모드)
    FILE *fp = fopen(fileName, "rb");
    if (fp == NULL) {
        perror("fopen input");
        exit(1);
    }

    // 매트릭스 읽어오기
    int matrix[MATRIX_SIZE][MATRIX_SIZE];
    if (fread(matrix, sizeof(int), MATRIX_SIZE * MATRIX_SIZE, fp) != MATRIX_SIZE * MATRIX_SIZE) {
        perror("fread");
        fclose(fp);
        exit(1);
    }
    fclose(fp);

    // 클라이언트 개수만큼 바이너리 파일 생성
    FILE *files[NUM_CLIENTS];
    for (int i = 0; i < NUM_CLIENTS; ++i) {
        char filename[256];
        snprintf(filename, sizeof(filename), "4x4_%d.bin", i);
        files[i] = fopen(filename, "wb"); // 바이너리 모드
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
                fwrite(&matrix[block_row * block_size + i][block_col * block_size], 
                       sizeof(int), block_size, files[file_index]);
            }
        }
    }

    // 파일 포인터 닫기
    for (int i = 0; i < NUM_CLIENTS; i++) {
        fclose(files[i]);
    }
}

// 8x8로 분할하고 바이너리 파일로 저장하는 함수
void split_matrix_8(const char* fileName)
{
    // 입력 파일 열기 (바이너리 읽기 모드)
    FILE *fp = fopen(fileName, "rb");
    if (fp == NULL) {
        perror("fopen input");
        exit(1);
    }

    // 매트릭스 읽어오기
    int matrix[MATRIX_SIZE][MATRIX_SIZE];
    if (fread(matrix, sizeof(int), MATRIX_SIZE * MATRIX_SIZE, fp) != MATRIX_SIZE * MATRIX_SIZE) {
        perror("fread");
        fclose(fp);
        exit(1);
    }
    fclose(fp);

    // 클라이언트 개수만큼 바이너리 파일 생성
    FILE *files[NUM_CLIENTS];
    for (int i = 0; i < NUM_CLIENTS; ++i) {
        char filename[256];
        snprintf(filename, sizeof(filename), "8x8_%d.bin", i);
        files[i] = fopen(filename, "wb"); // 바이너리 모드
        if (files[i] == NULL) {
            perror("fopen");
            for (int j = 0; j < i; ++j) {
                fclose(files[j]);
            }
            exit(1);
        }
    }
    
    int block_size = MATRIX_SIZE / 8;
    // 8x8 블록으로 나누어 각 클라이언트(파일)에 저장
    for (int block_row = 0; block_row < 8; ++block_row) {
        for (int block_col = 0; block_col < 8; ++block_col) {
            int file_index = (block_row * 8 + block_col) % NUM_CLIENTS;
            
            for (int i = 0; i < block_size; ++i) {
                fwrite(&matrix[block_row * block_size + i][block_col * block_size], 
                       sizeof(int), block_size, files[file_index]);
            }
        }
    }

    // 파일 포인터 닫기
    for (int i = 0; i < NUM_CLIENTS; i++) {
        fclose(files[i]);
    }
}