#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MSG_SIZE 100

int main() {
    int pipefd[2]; // 파이프 파일 디스크립터 배열
    pid_t pid;
    char write_msg[MSG_SIZE] ="Hello from child process!";
    char read_msg[MSG_SIZE];

    // 파이프 생성
    if (pipe(pipefd) ==-1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    // 자식 프로세스를 생성
    pid = fork();
    if (pid ==-1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid ==0) {
        // 자식 프로세스: 부모에게 메시지 전송
        close(pipefd[0]); // 읽기 끝 닫기
        printf("Child process: Sending message to parent pipe...\n");
        if (write(pipefd[1], write_msg, strlen(write_msg) +1) ==-1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        printf("Child process: Message sent successfully.\n");
        close(pipefd[1]); // 쓰기 끝 닫기
        exit(0); // 자식 프로세스 종료
    } else {
        // 부모 프로세스: 파이프로부터 메시지 수신
        close(pipefd[1]); // 쓰기 끝 닫기
        wait(NULL); // 자식 프로세스가 종료될 때까지 대기
        if (read(pipefd[0], read_msg, MSG_SIZE) ==-1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        printf("Parent process: Received message from child: %s\n", read_msg);
        close(pipefd[0]); // 읽기 끝 닫기
    }
    return 0;
}
