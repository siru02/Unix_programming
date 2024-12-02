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
    pid_t cPid = fork();
    checkForkError(cPid);

    if (cPid == 0) { // child
        cPid = fork();
        checkForkError(cPid);
        if (cPid == 0) { // child -> child
            cPid = fork();
            checkForkError(cPid);
            if (cPid == 0) { // child->child->child //sm0
                
            }
            else { // child->child->self //sm1

            }
        }
        else { //child->self
            cPid = fork();
            checkForkError(cPid);
            if (cPid == 0) { //child->self->child sm2

            }
            else { //child->self->self sm3

            }
        }
    }
    else { //self
        cPid = fork();
        checkForkError(cPid);
        if (cPid == 0) { // self -> child
            cPid = fork();
            checkForkError(cPid);
            if (cPid == 0) { //self->child->child sm4

            }
            else { //self->child->self sm5

            }
        }
        else { // self->self
            cPid = fork();
            checkForkError(cPid);
            if (cPid == 0) { //self->self->child sm6

            }
            else { //self->self->self sm7

            }
        }
    }
}