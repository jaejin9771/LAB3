#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    printf("부모 프로세스 시작: PID = %d\n", getpid());

    // 프로세스 생성
    pid = fork();

    if (pid < 0) {
        perror("fork 실패");
        exit(1);
    } else if (pid == 0) {
        // 자식 프로세스
        printf("자식 프로세스 시작: PID = %d, 부모 PID = %d\n", getpid(), getppid());
        
        // exec()로 새 프로그램 실행
        printf("자식 프로세스에서 다른 프로그램 실행 (ls 명령)\n");
        execl("/bin/ls", "ls", "-l", NULL);

        // exec() 호출 실패 시 처리
        perror("execl 실패");
        exit(1);
    } else {
        // 부모 프로세스
        printf("부모 프로세스에서 자식 프로세스 대기 중...\n");
        
        int status;
        wait(&status); // 자식 프로세스 종료 대기

        if (WIFEXITED(status)) {
            printf("자식 프로세스 종료. 종료 상태: %d\n", WEXITSTATUS(status));
        } else {
            printf("자식 프로세스 비정상 종료.\n");
        }

        printf("부모 프로세스 종료: PID = %d\n", getpid());
    }

    return 0;
}
