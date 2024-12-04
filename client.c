#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void checkForkError(pid_t pid) {
    if (pid < 0) {
        perror("fork");
        exit(1);
    }
}

int main() {
    pid_t pids[8]; // 8개의 프로세스 ID를 저장
    int client_num = 0;

    // for문으로 fork를 7번 수행하면 자식이7개 생성
    for (int i = 0; i < 7; i++) {
        pid_t cPid = fork();
        checkForkError(cPid);

        if (cPid == 0) { //자식프로세스의 동작으로 자신의 동작을 수행하고 fork를 추가로 실행하지 않고 종료되도록 마지막에 exit처리
            client_num = i; // 각 클라이언트 번호
            printf("Client Process %d (PID: %d, Parent PID: %d) started.\n", client_num, getpid(), getppid());
            sleep(1); // 클라이언트 작업 시뮬레이션
            printf("Client Process %d (PID: %d) exiting.\n", client_num, getpid());
            exit(0); // 작업 종료
        } else {
            // 부모 프로세스
            pids[i] = cPid; // 자식 PID 저장
        }
    }

    // 부모는 모든 자식의 종료를 기다림
    for (int i = 0; i < 7; i++) { //부모프로세스인 sm7은 대기할 필요가 없음
        waitpid(pids[i], NULL, 0); // 자식 프로세스 종료 대기
    }

    printf("All child processes exited. Parent Process (PID: %d) exiting.\n", getpid());
    return 0;
}
