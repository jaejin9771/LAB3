#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

// 시그널 핸들러 함수
void handle_signal(int sig) {
    if (sig == SIGUSR1) {
        printf("Received SIGUSR1 signal!\n");
    } else if (sig == SIGUSR2) {
        printf("Received SIGUSR2 signal!\n");
    } else if (sig == SIGINT) {
        printf("Received SIGINT (Ctrl+C), exiting...\n");
        exit(0);
    }
}

int main() {
    struct sigaction sa;
    sigset_t block_set;

    // sigaction 구조체 초기화
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;

    // SIGUSR1, SIGUSR2, SIGINT에 대해 핸들러 등록
    // 세번째 인자인 oldact는 사용하지 않아서 null로 지정한다
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    // 자식 프로세스를 생성하여 시그널 전송
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // 자식 프로세스: 부모에게 시그널 전송
        printf("Child process sending SIGUSR1 to parent...\n");
        kill(getppid(), SIGUSR1); // 부모 프로세스에 SIGUSR1 시그널 전송
        sleep(1); // 1초 대기
        printf("Child process sending SIGUSR2 to parent...\n");
        kill(getppid(), SIGUSR2); // 부모 프로세스에 SIGUSR2 시그널 전송
        exit(0); // 자식 프로세스 종료
    } else {
        // 부모 프로세스: SIGUSR1 블로킹 설정
        sigemptyset(&block_set);
        sigaddset(&block_set, SIGUSR1);
        sigprocmask(SIG_BLOCK, &block_set, NULL);
        printf("Parent process: SIGUSR1 is blocked.\n");

        // SIGUSR1 블로킹을 유지한 채로 시그널 대기
        while (1) {
            printf("Parent process waiting for signals...\n");
            pause(); // 시그널이 도착할 때까지 대기
        }
    }

    return 0;
}
