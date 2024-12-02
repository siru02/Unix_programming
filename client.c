#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <dirent.h>

void checkForkError(pid_t pid) {
    if (pid < 0) {
        perror("fork");
        exit(1);
    }
}

// 부모프로세스는 자식프로세스가 종료될 때까지 wait할것
int main()
{
    int client_num = 0;
    pid_t pid = getpid();
    pid_t cPid = fork(); //cPid는 fork시마다 계속 덮어쓰는 변수
    checkForkError(cPid);
    pid_t sm3 = cPid; // child->self->self

    if (cPid == 0) { // child
        cPid = fork();
        checkForkError(cPid);
        pid_t sm1 = cPid; //child->child->self

        if (cPid == 0) { // child -> child
            cPid = fork(); //child->child->child sm0
            checkForkError(cPid);

            if (cPid == 0) { // child->child->child //sm0
                
            }
            else { // child->child->self //sm1

                waitpid(cPid, NULL, 0); //직속 자식인 sm0의 종료를 기다린다
            }
        }
        else { //child->self
            cPid = fork();
            checkForkError(cPid);

            if (cPid == 0) { //child->self->child sm2

                waitpid(sm1, NULL, 0); //sm1의 종료를 기다린다
            }
            else { //child->self->self sm3

                waitpid(cPid, NULL, 0); //직속 자식인 sm2의 종료를 기다린다
            }
        }
    }
    else { //self
        pid_t sm3 = cPid; // child->self->self;
        cPid = fork();
        checkForkError(cPid);
        pid_t sm6 = cPid; //self->child->self sm5

        if (cPid == 0) { // self -> child
            cPid = fork();
            checkForkError(cPid);

            if (cPid == 0) { //self->child->child sm4

                waitpid(sm3, NULL, 0); //wait child->self->self sm3
            }
            else { //self->child->self sm5

                waitpid(cPid, NULL, 0); // wait self->child->child sm4
            }
        }
        else { // self->self
            pid_t sm5 = cPid; //self->child->self
            cPid = fork();
            checkForkError(cPid);

            if (cPid == 0) { //self->self->child sm6

                waitpid(sm5, NULL, 0); //wait self->child->self sm5
            }
            else { //self->self->self sm7

                waitpid(cPid, NULL, 0);
            }
        }
    }
}